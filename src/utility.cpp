#include "utility.h"
using namespace std;

void read_verilog(Abc_Frame_t * pAbc , string inputfile){
    char Command[1000];
    sprintf( Command, "read %s", inputfile.c_str());
    if ( Cmd_CommandExecute( pAbc, Command)){
        fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
    }
}
void transform2aig(Abc_Frame_t * pAbc){
    char Command[1000];
    sprintf( Command, "strash");
    if ( Cmd_CommandExecute( pAbc, Command ) ){
        fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
    }
}
void transform2ascii(string inputfile , string outputfile){
    aiger *init = aiger_init ();
    aiger_open_and_read_from_file(init , inputfile.c_str());
    aiger_open_and_write_to_file(init , outputfile.c_str());
    aiger_reset(init);
}
void write_aiger(Abc_Frame_t * pAbc , string outputfile){
    char Command[1000];
    sprintf( Command, "write_aiger %s", outputfile.c_str());
    if ( Cmd_CommandExecute( pAbc, Command ) ){
        fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
    }
}
void read_cnf(Abc_Frame_t * pAbc , string outputfile){
    char Command[1000];
    sprintf( Command, "write_cnf %s", outputfile.c_str());
    if ( Cmd_CommandExecute( pAbc, Command ) ){
        fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
    }
}
void print_suppset(Abc_Frame_t * pAbc){
    char Command[1000];
    sprintf( Command, "print_supp -s");
    if ( Cmd_CommandExecute( pAbc, Command ) ){
        fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
    }
}
void print_symmetries(Abc_Frame_t * pAbc){
    char Command[1000];
    sprintf( Command, "print_symm");
    if ( Cmd_CommandExecute( pAbc, Command ) ){
        fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
    }
}
void print_io(Abc_Frame_t * pAbc){
    char Command[1000];
    sprintf( Command, "print_io");
    if ( Cmd_CommandExecute( pAbc, Command ) ){
        fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
    }
}
void cec_sat(Abc_Frame_t * pAbc , string cir1 , string cir2){
    char Command[1000];
    sprintf( Command, "cec -s %s %s" ,cir1.c_str() , cir2.c_str());
    if ( Cmd_CommandExecute( pAbc, Command ) ){
        fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
    }
}
void d_sat(Abc_Frame_t * pAbc , string file_name){
    char Command[1000];
    sprintf( Command, "dsat %s" ,file_name.c_str());
    if ( Cmd_CommandExecute( pAbc, Command ) ){
        fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
    }
}
void print_func_suppset(Abc_Frame_t * pAbc){
    char Command[1000];
    sprintf( Command, "print_supp -v");
    if ( Cmd_CommandExecute( pAbc, Command ) ){
        fprintf( stdout, "Cannot execute command \"%s\".\n", Command );
    }
}
