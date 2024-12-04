#include <iostream>
#include "evaluate.h"
#include "statistic.h"
#include "utility.h"

using namespace std;

Abc_Frame_t * pAbc;
int main( int argc, char * argv[] ){
    int mod;
    string cirFile1, cirFile2, outputFile;
    // mod 1: 1:program
    // mod 2: 1:program 2:input file
    // mod 3: 1:output file 2: cir1 3 cir2
    if(argc < 2 || argc > 4){
        cout << "Wrong number of command-line arguments.\n";
        return 1;
    }else if(argc == 2){
        mod = 1;
    }else if(argc == 3){
        mod = 2;
    }else if(argc == 4){
        mod = 3;
        outputFile = argv[1];
        cirFile1 = argv[2];
        cirFile2 = argv[3];
    }
    Abc_Start();
    pAbc = Abc_FrameGetGlobalFrame();
    Report report = judge(outputFile, cirFile1, cirFile2);
    reportOutput(report);
    Abc_Stop();
    return 0;
}