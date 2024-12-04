#!/bin/bash
for ((i=0; i<=26; i++))
do
    echo -e ${i}
    time ./main ./inputFile/input${i}.txt ./outputFile/output${i}.txt	
done
