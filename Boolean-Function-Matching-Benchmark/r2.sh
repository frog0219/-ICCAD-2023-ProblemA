#!/bin/bash
for ((i=1; i<=9; i++))
do
    echo -e ${i}
    cp ./testCase2/case0${i}/* .
    /usr/bin/time -f "Elapsed time: %E" timeout 1h ./main input outputFile/output${i}
done
for ((i=10; i<=10; i++))
do
    echo -e ${i}
    cp ./testCase2/case${i}/* .
    /usr/bin/time -f "Elapsed time: %E" timeout 1h ./main input outputFile/output${i}
done
