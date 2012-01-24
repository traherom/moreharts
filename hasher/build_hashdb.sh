#!/bin/bash
DB_USER=root
DB_PWD=!n1jzw34.
DB_DATABASE=hashes
DB_HASH_TABLE=hashes

TMPISO=/tmp/nist.iso
TMPMNT=/tmp/nistisomount
TMPEX=/tmp/nistex
NSRLFILE="$TMPEX/NSRLFile.txt"
TMPSQLSCRIPT=/tmp/nistinsert.sql

# Must be run as root
if [ "$(whoami)" != "root" ]
then
	echo 'This must be run as root'
	exit 1
fi

# Check params
if [ "$#" == "0" ]
then
	echo 'ISO(s) must be given to download'
	exit 2
fi

# Download each ISO in turn
for f in $@
do
	# Determine how to handle it
	ext=$($f | awk -F . '{print $NF}' )
	
	# Download
	if [ $($f | awk -F ':' '{print $1}') == "http" ]
	then
		echo Dowloading $f
		wget "$f" "$TMPISO"
		sha1sum "$TMPISO"
		
		# Prepare for the next step
		downloaded=1
		f=$TMPISO
		ext="iso"
	fi
	
	# Mount
	if [ $ext == "iso" ]
	then
		echo Mounting ISO
		if [ ! -d "$TMPMNT" ]
		then
			mkdir "$TMPMNT"
		fi
		
		mount -t iso9660 -o ro "$TMPISO" "$TMPMNT"
		
		mounted=1
		f="$TMPMNT/*.zip"
		ext="zip"
	fi
	
	# Extract
	if [ ext == "zip" ]
	then
		echo Extracting contents
		unzip $f -d "$TMPEX"
		
		extracted=1
		f="$TMPEX"
		ext=""
	fi
	
	# Unmount if needed
	if [ $mounted ]
	then
		umount "$TMPMNT"
		rmdir "$TMPMNT"
	fi
	
	# Build SQL
	echo Building SQL commands
	echo "CREATE DATABASE IF NOT EXISTS $DB_DATABASE;" >"$TMPSQLSCRIPT" 
	echo "USE $DB_DATABASE;" >>"$TMPSQLSCRIPT"
	
	for table_file in $(ls "$f")
	do
		# Skip hashes file, it's not a nice CSV
		if [ $table_file == "hashes.txt" ]; continue
		
		table_name=$(echo $table_file | sed s/\.txt//)
		table_file_path="$TMPEX/$table_file"
		
		echo "CREATE TABLE IF NOT EXISTS $table_name (" >>"$TMPSQLSCRIPT"
	
		head -n 1 "$table_file_path" |
			awk '{ n = split($0, cols, /,/);
				   for (i = 1; i < n; i++)
				   {
					 gsub(/[^a-zA-Z0-91]/, "", cols[i]);
					 printf cols[i] " varchar(1024)";
					 if ( i != n - 1 ) printf ","
					 printf ORS
				   }
				 }' >>"$TMPSQLSCRIPT"
	
		echo ");" >>"$TMPSQLSCRIPT" >>"$TMPSQLSCRIPT"
		echo "LOAD DATA LOCAL INFILE \"$table_file_path\" REPLACE INTO TABLE $table_name COLUMNS TERMINATED BY ',' ENCLOSED BY '\"' IGNORE 1 LINES;" >>"$TMPSQLSCRIPT"
	done
	
	echo Inserting
	mysql "--user=$DB_USER" "--password=$DB_PWD" -B <"$TMPSQLSCRIPT"
	
	# Cleanup
	echo Completed $f, cleaning up
	if [ $extracted ]; rm -r "$TMPEX"
	if [ $downloaded ]; rm "$TMPISO"
done

