#!/bin/bash
# Test each mode (0-2) with 1 through 8 processors
# and run each test multiple times with differing
# numbers of max primes

maxProc=8
repeatCount=1

# Ensure we're testing the latest build
make 1>&2
if [ $? -ne 0 ]
then
	echo "Build failed, fix that first" 1>&2
	exit
fi

# Varying problem sizes
for prime in 1000 10000 100000 1000000 10000000 100000000 1000000000
do
	# Headers for columns
	echo "Max Prime, Mod#, Primes, Processor Count, Time (s)"
	
	# Each mode type
	for mode in {0..2}
	do
		# 1-8 processors
		for((procCount = 1; procCount <= maxProc; procCount++))
		do
			# Repeat so we could average if desired
			for((rep = 0; rep < repeatCount; rep++))
			do
				echo Max prime: $prime, Mode: $mode, Processors: $procCount 1>&2
				mpirun -np $procCount -hostfile lamHosts ./framework -t $mode $prime
			done
		done 
	done
	
	# newline
	echo
done

