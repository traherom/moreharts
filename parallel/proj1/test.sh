#!/bin/sh

# This shell script runs the single-processor version of project one and gathers
# data about the return values and times, outputing them to a CSV file.

intervals=100000000
increment_size=1000000
start=60000000

# Rectangle approximation, single threaded
# Remove the old CSV file
#rm single_rects.csv 2>/dev/null

# Define the loop to run for 1,000,000 intervals in a single-processor model
i=$start
while [ $i -le $intervals ]
do
	echo Single Rects, intervals: $i

	# Execute the pi calculation for each interval and output to CSV file
	./pi_s $i >> single_rects.csv
	i=$((i+$increment_size))
done

# Rectangle approximation, parallel
#rm parallel_rects.csv 2>/dev/null
i=$start
while [ $i -le $intervals ]
do
	echo Parallel Rects, intervals: $i
	mpirun -v -np 8 -hostfile lamHosts pi_p $i >> parallel_rects.csv
	i=$((i+$increment_size))
done

# Simpson's, single
#rm single_simpsons.csv 2>/dev/null
i=$start
while [ $i -le $intervals ]
do
	echo Single Simpsons, intervals: $i
	./pi_simpsons_single $i >> single_simpsons.csv
	i=$((i+$increment_size))
done

# Simpson's, parallel
#rm parallel_simpsons.csv 2>/dev/null
i=$start
while [ $i -le $intervals ]
do
	echo Parallel Simpsons, intervals: $i
	mpirun -v -np 8 -hostfile lamHosts pi_simpsons_mpi $i >> parallel_simpsons.csv
	i=$((i+$increment_size))
done

