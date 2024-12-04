#include "circuit.h"
#include "timer.h"
#include <algorithm>
#include <stack>
bool Incremental_SAT_based_Solver(Circuit cir1 , Circuit cir2  , stack<pair<int , int>> output_match_set , ofstream &DeBug ,Timer start_time ,  string outputfile , vector<vector<int>> &learn_clause);
bool modify_cnf(Circuit cir1 , Circuit cir2 , vector<pair<int , int>> match_out , ofstream &DeBug , CaDiCaL::Solver *mapping, CaDiCaL::Solver *miter);
void write_ans(Circuit cir1 , Circuit cir2 ,string output_file_name);
bool generate_matching_circuit_cnf(Circuit cir1 , Circuit cir2  , vector<int> add_cnf , ofstream &DeBug , CaDiCaL::Solver *mapping , CaDiCaL::Solver *miter);
void init_input_cnf(Circuit cir1 , Circuit cir2,ofstream &DeBug , CaDiCaL::Solver *mapping ,vector<vector<int>> learn_clause);
set<int>  gen_redomdam_set(Circuit cir2 , vector<int> counter_example , int output_ans , int id);