#!/bin/bash
zeros="000000000"

for s in {1..10}
do
    size="$s$zeros"
    ./main0 file1.txt $size
    echo "size $s, sequential mode"
done

for s in {1..10}
do
	for t in {1..16}
	do
		size="$s$zeros"
		./main file1.txt $size $t
		echo "size $s, threads $t" 
	done
done
