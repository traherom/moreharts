#!/bin/bash
CONVERTER=~/bin/video_converter
LOCK_FILE=~/.conversion_monitor
DOWNLOAD_LOCK_FILE=~/.download_monitor
UNCONVERTED=~/downloads/unconverted
CONVERTED=~/downloads/converted
MAILER=~/bin/gmailer

# Don't run if the download monitor is still copying stuff
# Also require a clean exit, make it remove its own file
if [ -e $DOWNLOAD_LOCK_FILE ] ; then
	echo Download monitor still running, they must finish first
	exit 1
fi

# Only one instance can run
if [ -e $LOCK_FILE ] ; then
	# Process actually running still?
	if ps `cat $LOCK_FILE` | grep -v COMMAND ; then
		echo Monitor already running
		exit 1
	fi
fi

# Write out PID
echo $$ > $LOCK_FILE

# Any files to process?
while IFS= read -d $'\0' -r f; do
	echo Converting "$f"...
	"$CONVERTER" --remove-source --remove-failed "$f" "$CONVERTED"

	echo Emailing status!
	if [ "$?" == "0" ] ; then
		"$MAILER" --from ryan@moreharts.com --to ryan@moreharts.com --to caitlin@moreharts.com --subject "Video conversion complete" --body "$f is now available for viewing"	
	else
		"$MAILER" --from ryan@moreharts.com --to ryan@moreharts.com --to caitlin@moreharts.com --subject "Video conversion failed" --body "$f failed conversion. Please try a different download."
	fi
done < <(find "$UNCONVERTED" \( -name \*.avi -o -name \*.mp4 \) | sort | tr '\n' '\000')

# Unlock
rm $LOCK_FILE

