#include <string.h>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <ctime>
#include <iomanip>
#include <chrono>
#include "aiger.h"
#include "cadical.hpp"
using namespace std;

#ifndef UTILITY_H
#define UTILITY_H
void  transform2ascii(string inputfile , string outputfile);

#if defined(ABC_NAMESPACE)
namespace ABC_NAMESPACE
{
#elif defined(__cplusplus)
extern "C"
{
#endif

typedef struct Abc_Frame_t_ Abc_Frame_t;
void   Abc_Start();
void   Abc_Stop();
Abc_Frame_t * Abc_FrameGetGlobalFrame();
int Cmd_CommandExecute( Abc_Frame_t * pAbc, const char * sCommand );

void read_verilog(Abc_Frame_t * pAbc , string inputfile);
void write_aiger(Abc_Frame_t * pAbc , string outputfile);
void transform2aig(Abc_Frame_t * pAbc);
void print_suppset(Abc_Frame_t * pAbc);
void print_func_suppset(Abc_Frame_t * pAbc);
void read_cnf(Abc_Frame_t * pAbc , string outputfile);
void print_symmetries(Abc_Frame_t * pAbc);
void print_io(Abc_Frame_t * pAbc);
void cec_sat(Abc_Frame_t * pAbc , string cir1 , string cir2);
void d_sat(Abc_Frame_t * pAbc , string file_name);

#if defined(ABC_NAMESPACE)
}
using namespace ABC_NAMESPACE;
#elif defined(__cplusplus)
}
#endif

#endif