#!/bin/bash

result="result"
l="-L"
d="-D"
txt=".txt"

for (( i=8; i<=512; i=i*2));
do
	for (( j=8; j<=128; j=j*2));
	do
		r=$result"/"${result}${l}${i}${d}${j}${txt}
		s="$result$l$i$d$j$txt"
		./avg $r
		echo $i $j
	done
done
