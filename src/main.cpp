#include<iostream>
#include <fstream>
#include<vector>
#include "output_solver.h"
using namespace std;
Circuit cir1 , cir2;
void read_inputfile(string path);
int main( int argc, char * argv[] ){
    string filename , outfile;
    if(argc == 3){
        filename =  argv[1];
        outfile = argv[2];
    }
    else {
        cout << "Wrong number of command-line arguments.\n";
        return 1;
    }
    Timer t;
    
    read_inputfile(filename);
    output_solver(cir1 , cir2 ,t ,outfile);
    cout << (float)t.GetElapsedMillseconds() / 1000.0 << '\n';
    cout << cir1.func_num << '\n';
    return 0;
}
void read_inputfile(string path){
    ifstream inputfile(path);
    string circuit;
    if (inputfile.is_open()) {

       inputfile >> circuit ;
       cir1.get_filename(circuit);
       cir1.read_circuit();
       cir1.read_bus(&inputfile);

       inputfile >> circuit;
       cir2.get_filename(circuit);
       cir2.read_circuit();
       cir2.read_bus(&inputfile);
    }
    inputfile.close();
}