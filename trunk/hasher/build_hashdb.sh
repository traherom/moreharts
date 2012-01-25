#!/bin/bash
DB_USER=root
DB_PWD=!n1jzw34.
DB_DATABASE=hashes

# Listing url must end in slash
NSRL_LISTING_URL="http://www.nsrl.nist.gov/RDS/"
ISO_HASH_FILE="iso_hash.txt"

TMPMNT="nist_iso_mount_point"
TMPEX="nist_iso_zip_extracted"
TMPSQLSCRIPT="nist_insert_script.sql"

DB_INSERT_INCREMENT_SIZE=1000

# Must be run as root
if [ "$(whoami)" != "root" ]
then
	echo 'This must be run as root'
	exit 1
fi

# Clean up from failed attempt?
if [ -e "$TMPMNT" ]
then
	echo "Mount point $TMPMNT appears to still exist"
	echo -n "Attempting to clean up... "
	umount "$TMPMNT"
	rmdir "$TMPMNT"
	if [ ! -d "$TMPMNT" ]
	then
		echo "ok"
	else
		echo "failed. Please fix manually"
		exit
	fi
fi

if [ -e "$TMPEX" ]
then
	echo "Extract directory $TMPEX still exists"
	echo -n "Attempting to clean up... "
	rm "$TMPEX"/*.txt
	rmdir "$TMPEX"
	if [ ! -d "$TMPEX" ]
	then
		echo "ok"
	else
		echo "failed. Please fix manually"
		exit
	fi
fi

if [ -e "$TMPSQLSCRIPT" ]
then
	echo "SQL script $TMPSQLSCRIPT still exists"
	echo -n "Attempting to clean up... "
	rm "$TMPSQLSCRIPT"
	if [ ! -e "$TMPSQLSCRIPT" ]
	then
		echo "ok"
	else
		echo "failed. Please fix manually"
		exit
	fi
fi

# Did the user give us a file with the SHA1s for the
# ISOs they want to use?
if [ "$#" != "0" ]
then
	if [ -f "$1" ]
	then
		echo "Using $1 for ISO hashes"
		ISO_HASH_FILE="$1"
	
		downloaded="false"
	else
		echo "The ISO hash file $1 does not exist"
		exit 12
	fi
else
	# Determine newest version
	echo "Checking for the newest version of hash files"
	newest=`wget -O - -q $NSRL_LISTING_URL | tail -n 1 | awk '{match($0, /rds_[0-9]+\.[0-9]+/); print substr($0, RSTART, RLENGTH);}'`
	base_url="$NSRL_LISTING_URL$newest"

	echo "Newest version appears to be $newest"
	echo "Should be at $base_url"
	
	# Download hash file
	echo "Getting hashes for newest ISOs"
	rm -f "$ISO_HASH_FILE"
	wget -q "$base_url/$ISO_HASH_FILE" "$ISO_HASH_FILE"
	
	downloaded="true"
fi

# Get hashes and compare to current files, if they exist
OLD_IFS=$IFS
IFS=`echo -en "\r\n"`
for iso_hash_line in `grep "SHA1" "$ISO_HASH_FILE"`
do
	# NOTE: iso_hash_line has a newline before, probably.
	# No need to clean though
	iso_file_name=$(echo $iso_hash_line | awk '{match($0, /RDS_[0-9]+_[a-zA-Z]+\.iso/); printf substr($0, RSTART, RLENGTH);}')
	iso_hash=$(echo $iso_hash_line | awk '{match($0, /= [a-zA-Z0-9]+/); printf substr($0, RSTART + 2, RLENGTH - 2);}')
	
	# Does the file exist?
	echo "Checking if $iso_file_name exists already"
	if [ ! -e $iso_file_name ]
	then
		if [ "$downloaded" == "true" ]
		then
			# No, download
			echo "Downloading $iso_file_name"
			wget "$base_url/$iso_file_name" "$iso_file_name"
		else
			# We don't have it, but we also can't download
			echo "ISO $iso_file_name doesn't exist, but running with specific hash file"
			exit 9
		fi
	fi
	
	# Check hash
	echo -n "Checking hash of $iso_file_name... "
	if [ `sha1sum "$iso_file_name" | awk '{printf $1}'` == "$iso_hash" ]
	then
		echo "ok"
	else
		echo "hash mismatch"
		echo "Please remove $iso_file_name. The SHA1 should be $iso_hash"
		rm -i "$iso_file_name"
		exit 5
	fi 
done

# Restore the IFS
IFS=$OLD_IFS

# Create database
echo "Creating database if needed"
echo "CREATE DATABASE IF NOT EXISTS $DB_DATABASE;
	USE $DB_DATABASE;
	CREATE TABLE IF NOT EXISTS NSRLProd (
		ProductCode int NOT NULL,
		ProductName varchar(1024),
		ProductVersion varchar(1024),
		OpSystemCode varchar(255),
		MfgCode varchar(100),
		Language varchar(1024)
	) ENGINE=innodb;
	CREATE TABLE IF NOT EXISTS NSRLFile (
		SHA1 char(41),
		MD5 char(32),
		CRC32 char(32),
		FileName varchar(1024),
		FileSize varchar(1024),
		ProductCode int,
		OpSystemCode varchar(255),
		Specialcode varchar(255)
	) ENGINE=innodb;" | mysql "--user=$DB_USER" "--password=$DB_PWD" -B
		
for iso in `ls -1 *.iso`
do
	# Mount
	echo "Mounting '$iso'"
	if [ -d "$TMPMNT" ]
	then
		echo "$TMPMNT already exists already, please remove and try again"
		rm -i "$TMPMNT"
		exit 7
	fi
	mkdir "$TMPMNT"
	mount -t iso9660 -o ro "$iso" "$TMPMNT"
	
	# Extract
	echo "Extracting contents"
	unzip "$TMPMNT"/*.zip -d "$TMPEX"
	
	# Unmount
	echo "Unmounting ISO"
	umount "$TMPMNT"
	rmdir "$TMPMNT"
	
	# Check hashes of individual files compared to hashes.txt
	for nsrl_file in `ls -1 "$TMPEX"/NSRL*.txt`
	do
		echo -n "Checking hash of $nsrl_file..."
		
		if [ $(grep `sha1sum "$nsrl_file" | awk '{printf toupper($1)}'` "$TMPEX/hashes.txt") != "" ]
		then
			echo "ok"
		else
			echo "failed"
			echo "$nsrl_file does not match the hash for it given in hashes.txt"
			echo "The extraction may have failed, please try again"
			exit
		fi
	done
	
	# Insert product data
	echo "Inserting product codes into database"
	echo "LOAD DATA LOCAL INFILE \"$TMPEX/NSRLProd.txt\"
		REPLACE INTO TABLE hashes.NSRLProd
		COLUMNS TERMINATED BY ','
		ENCLOSED BY '\"' IGNORE 1 LINES;" | mysql "--user=$DB_USER" "--password=$DB_PWD" -B
	
	#	LOAD DATA LOCAL INFILE \"$TMPEX/NSRLFile.txt\"
	#		REPLACE INTO TABLE NSRLFile
	#		COLUMNS TERMINATED BY ','
	#		ENCLOSED BY '\"' IGNORE 1 LINES;"
	
	# Insert hashes
	total_hashes=`cat "$TMPEX"/NSRLFile.txt | wc -l`
	curr_hash=-1
	echo "There are $total_hashes hashes to insert"
	while read line
	do
		# Skip the very first line
		if [ "$curr_hash" == "-1" ]
		then
			curr_hash=0
			continue
		fi
		
		# Mark what hash we started with
		start_hash=$curr_hash
		
		# Get a bunch of hashes at once
		echo "INSERT INTO hashes.NSRLFile VALUES ($line);" >"$TMPSQLSCRIPT"
		for i in `seq 1 $DB_INSERT_INCREMENT_SIZE`
		do
			curr_hash=`expr $curr_hash + 1`
			
			read line
			if [ "$line" != "" ]
			then
				echo "INSERT INTO hashes.NSRLFile VALUES ($line);" >>"$TMPSQLSCRIPT"
			else
				# All done with file, I guess
				break
			fi		
		done
		
		echo "Inserting hashes $start_hash-$curr_hash of $total_hashes..."
		
		mysql "--user=$DB_USER" "--password=$DB_PWD" -B <"$TMPSQLSCRIPT"
	done <"$TMPEX/NSRLFile.txt"
	
	# Cleanup
	echo Completed $iso, cleaning up
	rm -r "$TMPEX"
	rm "$TMPSQLSCRIPT"
done

# Add indexes
echo "Adding indexes to database, please wait, this may take 30 minutes or more. Alsa, no progress bar"
echo "ALTER TABLE hashes.NSRLFile
		ADD CONSTRAINT UNIQUE INDEX (SHA1),
		ADD CONSTRAINT UNIQUE INDEX (MD5);
	ALTER TABLE hashes.NSRLProd
		ADD CONTSRAINT UNIQUE INDEX (ProductCode);" | mysql "--user=$DB_USER" "--password=$DB_PWD" -B

echo "All done!"

