#!/bin/bash
CONVERTER=~/bin/video_converter
LOCK_FILE=~/.conversion_monitor
DOWNLOAD_LOCK_FILE=~/.download_monitor
UNCONVERTED=~/downloads/unconverted
CONVERTED=~/downloads/converted

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
if ls -1 $CONVERTED | grep ^ ; then
	echo Starting conversion...
	$CONVERTER --remove-source --remove-failed $UNCONVERTED $CONVERTED
fi

# Unlock
rm $LOCK_FILE

