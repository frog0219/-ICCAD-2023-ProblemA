#!/bin/bash
for ((i=1; i<=9; i++))
do
    echo -e ${i}
    ./main outputFile/output${i} testCase2/case0${i}/circuit_1.v testCase2/case0${i}/circuit_2.v
done
for ((i=10; i<=10; i++))
do
    echo -e ${i}
    ./main outputFile/output${i}  testCase2/case${i}/circuit_1.v testCase2/case${i}/circuit_2.v
done
