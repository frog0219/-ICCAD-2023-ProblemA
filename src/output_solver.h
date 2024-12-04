#include"input_solver.h"
using namespace std;
pair<vector<vector<Output>> , vector<vector<Output>>> init_output_group(Circuit cir1 , Circuit cir2);
bool find_new_pair_group(vector<vector<Output>> group_cir1 , vector<vector<Output>> group_cir2 , vector<int> &cir2_match_group_id , vector<bool> &cir2_match_possitive ,vector<bool> &cir1_use);
void output_solver(Circuit cir1 , Circuit cir2 , Timer start_time , string outputfile);

