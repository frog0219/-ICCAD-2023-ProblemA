//
// Created by grorge on 4/14/23.
//

#include <string.h>
#include <iostream>
using namespace  std;

#ifndef JUDGE_STATISTIC_H
#define JUDGE_STATISTIC_H
struct Report{
    string cir1, cir2;
    time_t time;
    bool passed;
    double score;
};

void reportOutput(Report report);

#endif //JUDGE_STATISTIC_H
