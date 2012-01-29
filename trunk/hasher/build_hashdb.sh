#!/bin/bash
# Don't allow the use of uninitialized variables
set -o nounset

# Settings
DB_HOST=localhost
DB_USER=root
DB_PWD=test
DB_DATABASE=hashes

# Listing url must end in slash
NSRL_LISTING_URL="http://www.nsrl.nist.gov/RDS/"
ISO_HASH_FILE="iso_hash.txt"

TMPMNT="nist_iso_mount_point"
TMPEX="nist_iso_zip_extracted"

SQL_CMD="mysql --host=$DB_HOST --user=$DB_USER --password=$DB_PWD --database=$DB_DATABASE -B "

# Utility function to clean up the mess we make
function clean_up_temp () {
	if [ -e "$TMPMNT" ]
	then
		echo -n "Attempting to unmount/remove $TMPMNT clean up... "
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
		echo -n "Attempting to clean up $TMPEX... "
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
}

# Exit cleanly
SQL_PROC_ID=""
function clean_exit () {
	echo "Cleaning up"
	
	# Kill background procs if needed
	if [ "$SQL_PROC_ID" != "" ]
	then
		echo "Asking mysql to exit"
		kill $SQL_PROC_ID
		wait $SQL_PROC_ID 
	fi
	
	# Clean up the temporary files we make
	echo "Removing temporary files"
	clean_up_temp
	
	# And really finish
	trap - int term exit
	exit
}
trap clean_exit int term exit

# Must be run as root
if [ "$(whoami)" != "root" ]
then
	echo 'This must be run as root'
	exit 1
fi

# Test database connection
echo -n "Testing database connection... "
output=`$SQL_CMD -e "select 1;" 2>&1`
if [ "$?" == "0" ]
then
	echo "ok"
else
	echo "failed"
	echo $output
	echo "Database settings can be changed by editing variables at the top of this script"
	exit 123
fi

# Clean up from failed attempt?
clean_up_temp

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

	# Did we figure it out?
	if [ "$newest" == "" ]
	then
		echo "Unable to determine newest version. Is your network up?"
		exit 17
	fi

	echo "Newest version appears to be $newest"
	echo "Should be at $base_url"
	
	# Download hash file
	echo "Getting hashes for newest ISOs"
	rm -f "$ISO_HASH_FILE"
	wget -q "$base_url/$ISO_HASH_FILE"
	if [ "$?" != "0" ]
	then
		echo "Failed to download iso hash file. Has the location been moved?"
		echo "You may need to manually download and give the location on the command line"
		exit 22
	fi
	
	downloaded="true"
fi

# Get hashes and compare to current files, if they exist
iso_files=
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
		iso_files="$iso_files $iso_file_name"
	else
		echo "hash mismatch"
		echo "Please remove $iso_file_name. The SHA1 should be $iso_hash"
		rm -i "$iso_file_name"
		exit 5
	fi 
done

# Restore the IFS
IFS=$OLD_IFS

# Create database tables
# We're really only meant to refresh the hashes, we don't really
# just add to them, so clean everything out
echo "Preparing database"
echo "DROP TABLE IF EXISTS NSRLProd;
	CREATE TABLE IF NOT EXISTS NSRLProd (
		ProductCode int unsigned NOT NULL,
		ProductName varchar(1024),
		ProductVersion varchar(1024),
		OpSystemCode varchar(255),
		MfgCode varchar(100),
		Language varchar(1024),
		CONSTRAINT UNIQUE INDEX (ProductCode)
	) ENGINE=innodb;
	DROP TABLE IF EXISTS NSRLFile;
	CREATE TABLE IF NOT EXISTS NSRLFile (
		SHA1 char(41),
		MD5 char(32),
		CRC32 char(32),
		FileName varchar(1024),
		FileSize varchar(1024),
		ProductCode int unsigned,
		OpSystemCode varchar(255),
		Specialcode varchar(255),
		INDEX sha_index (SHA1)
	) ENGINE=innodb;
	CREATE TABLE IF NOT EXISTS systems (
		comp_id int unsigned NOT NULL AUTO_INCREMENT,
		name varchar(100) NOT NULL,
		PRIMARY KEY (comp_id),
		UNIQUE INDEX (name)
	) ENGINE=innodb;
	CREATE TABLE IF NOT EXISTS current_files (
		comp_id int unsigned NOT NULL,
		SHA1 char(41) NOT NULL,
		path varchar(1024) NOT NULL,
		found bit DEFAULT false,
		FOREIGN KEY (comp_id) REFERENCES systems (comp_id),
		INDEX (comp_id, SHA1),
		UNIQUE INDEX (comp_id, path),
	) ENGINE=innodb;" | $SQL_CMD

# Having the key in place for the insert will be slow as crap
if [ "$iso_files" != "" ]
then
	echo "ALTER TABLE NSRLFile DROP INDEX sha_index;" | $SQL_CMD
fi

# Now work through each ISO!
for iso in $iso_files
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
		INTO TABLE NSRLProd
		COLUMNS TERMINATED BY ','
		ENCLOSED BY '\"' IGNORE 1 LINES;" | $SQL_CMD &
	
	function monitor_insert () {
		# Allow mysql to be killed if needed
		SQL_PROC_ID=$!

		total_count=$1
		insert_count=0
		time_left=99999
		
		# Get the length of the total count so we know how much to pad our output by
		len_total=${#total_count}
		backspace_str=""
		for i in $(seq 1 `expr $len_total \* 2 + 25`)
		do
			backspace_str=$backspace_str"\b"
		done
		
		printf "Inserted "
		while [ "$insert_count" != "" ]
		do
			printf "%${len_total}d / %${len_total}d (%5d min remaining)" $insert_count $total_count $time_left
			sleep 5
			
			# Get status of load
			db_status=`$SQL_CMD -e "SHOW INNODB STATUS;"`
			
			# How many are done so far?
			insert_count=$(echo $db_status | awk '{match($0, /undo log entries [0-9]+/);
												   printf substr($0, RSTART + 17, RLENGTH - 17)}')
			
			if [ "$insert_count" != "" ]
			then
				# Compute time remaining
				inserts_per_sec=$(echo $db_status | awk '{match($0, /[0-9]+\.[0-9]+ inserts\/s/);
														  n=substr($0, RSTART, RLENGTH - 10);
														  match(n, /[0-9]+\./);
														  printf substr(n, RSTART, RLENGTH - 1);}')
			
				if [ "$inserts_per_sec" != "" -a "$inserts_per_sec" != "0" ]
				then
					time_left=`expr \( $total_count - $insert_count \) / $inserts_per_sec / 60`
				else
					time_left=0
				fi
			else
				# No more inserts, we must have finished
				time_left=0
			fi
			
			# Back up display for dynamic output
			printf "$backspace_str"
		done
		
		# Fully erase the Inserting... line
		printf "$backspace_str$backspace_str"
		
		# Make sure mysql is done
		wait
		
		# No need to force now
		SQL_PROC_ID=""
	}
	monitor_insert `wc -l $TMPEX/NSRLProd.txt`
	
	echo "Inserting hashes into database. This will take a long time, be patient"
	echo "LOAD DATA LOCAL INFILE \"$TMPEX/NSRLFile.txt\"
			INTO TABLE NSRLFile
			COLUMNS TERMINATED BY ','
			ENCLOSED BY '\"' IGNORE 1 LINES;" | $SQL_CMD &
	monitor_insert `wc -l $TMPEX/NSRLFile.txt`
	
	# Cleanup
	echo Completed $iso
	rm -r "$TMPEX"
done

# Add index back to table
if [ "$iso_files" != "" ]
then
	echo "Reindexing table. Please wait, this could take a while"
	echo "ALTER TABLE NSRLFile ADD INDEX sha_index (SHA1);" | $SQL_CMD
fi

