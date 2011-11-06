#!/bin/bash
LOCK_FILE=~/.download_monitor
COMPLETE=~/downloads/complete/
UNCONVERTED=~/downloads/unconverted/

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

# Look for any complete video files that just finished and copy them for conversion
# Once we copy them, mark them to not be copied again
for f in $( find $COMPLETE -name \*.avi -mmin -720 -size 50M ); do
	echo Copying $f
	cp $f $UNCONVERTED
	touch --date="yesterday" $f
done
	
# Unlock for next round
rm $LOCK_FILE

