#!/bin/bash
# Ensure we're testing the latest build
javac -d classes src/proj6/*.java
cd classes

outputFile=../test.out
maxValue=1000
random=true
size=25

echo ::Show correctness of algorithms:: > $outputFile
java proj6/Project6 1 -m $maxValue -r $random -s $size >> $outputFile
java proj6/Project6 2 -m $maxValue -r $random -s $size >> $outputFile
for procCount in 1 2 3 4
do
	java proj6/Project6 3 -m $maxValue -r $random -s $size -n $procCount >> $outputFile
done

echo >> $outputFile

echo ::Run benchmark tests:: >> $outputFile
for part in 1 2
do
	for size in 100 1000 5000 10000 50000 100000 500000 1000000
	do
		java proj6/Project6 $part -m $maxValue -r $random -s $size >> $outputFile
	done

	echo >> $outputFile
done
for size in 100 1000 5000 10000 50000 100000 500000 1000000 5000000 10000000
do
	for procCount in 1 2 3 4
	do
		java proj6/Project6 3 -m $maxValue -r $random -s $size -n $procCount >> $outputFile
	done
done
echo >> $outputFile

