#!/bin/bash
for i in 1 2 4 5 7 8 9
do
    echo -e ${i}
    cp ./testCase2/case0${i}/* .
    /usr/bin/time -f "Elapsed time: %E" timeout 3m ./main input outputFile/output${i}
done
exit
for i in 10
do
    echo -e ${i}
    cp ./testCase2/case${i}/* .
    /usr/bin/time -f "Elapsed time: %E" timeout 3m ./main input outputFile/output${i}
done
