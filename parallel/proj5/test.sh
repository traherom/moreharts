#!/bin/bash
# Ensure we're testing the latest build
make 1>&2
if [ $? -ne 0 ]
then
	echo "Build failed, fix that first" 1>&2
	exit
fi

# Varying problem sizes
for prime in 100 10000 1000000 100000000 250000000 500000000 1000000000
do
	# Headers for columns
	echo "Max Prime, Threads, Time, Prime Count"
	
	# 1-32 threads
	for threadCount in {1..32}
	do
		echo Max prime: $prime, Threads: $threadCount 1>&2
		./threaded -t $threadCount $prime
	done 
	
	# newline
	echo
done

