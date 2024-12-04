//
// Created by grorge on 4/14/23.
//
#include "statistic.h"
#include "utility.h"

void reportOutput(Report report){
    cout << "[" << formatTime(report.time) << "] " << report.cir1 << " " << report.cir2 << " equivalent test: "
    << (report.passed ? "passed" : "failed") << endl;
    cout << "Score: " << report.score << endl;
}