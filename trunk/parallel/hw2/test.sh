#!/bin/bash
for((i = 1; i <= $1; i++))
do
	./reduction $1 $i
	echo ""
done

