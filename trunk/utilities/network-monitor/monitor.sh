#!/bin/bash
NESSUS_FILE="settings.nessus.v1"
NESSUS_POLICY="Internal Network Scan"

TARGETS="10.1.30.2"

SCANNER_HOST="localhost"
SCANNER_PORT=1241
SCANNER_USER="cdx"
SCANNER_PW="Password!123"

LAST_RESULT_FILE=

WAIT_TIME=5

# Write out targets to a file for use in call
TARGET_FILE="targets.txt"
echo $TARGETS > "$TARGET_FILE"

# Initial status
echo Started `date`

while true
do
	# Build file name
	RESULT_FILE=`date +%F-%T`
	RESULT_FILE="${RESULT_FILE}.out"

	# Run Nessus
	echo -en "\r                                  \rScanning"
	/opt/nessus/bin/nessus -q -T text --dot-nessus "$NESSUS_FILE" \
		--policy-name "$NESSUS_POLICY" --target-file "$TARGET_FILE" "$SCANNER_HOST" \
		"$SCANNER_PORT" "$SCANNER_USER" "$SCANNER_PW" "$RESULT_FILE"

	# Compare
	echo -en "\r                                  \rChecking"
	if [ "$LAST_RESULT_FILE" != "" ]
	then
		DIFF_RES=`diff "$LAST_RESULT_FILE" "$RESULT_FILE"`
		if [ "$DIFF_RES" != "" ]
		then			
			# Changes detected!
			echo -e "\r                                  \r--- CHANGES DETECTED ---"
			echo "----------------------"
			date
			echo "$DIFF_RES"
			echo "----------------------"
		fi
	fi

	# Ready for next time
	echo -en "\r                                  \rWaiting"
	LAST_RESULT_FILE="$RESULT_FILE"
	sleep $WAIT_TIME
done

