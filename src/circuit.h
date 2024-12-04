#include<iostream>
#include<vector>
#include<map>
#include "utility.h"
#include <fstream>
#include <queue>
using namespace std;

struct Node{
    Node(int id , int parent1 , int parent2  , bool positive1 , bool positive2 , bool cur_positive , int layer):
        id(id) , parent1(parent1) , parent2(parent2) , positive1(positive1) , positive2(positive2) , cur_positive(cur_positive) , layer(layer){}
    Node(){}
    int id , parent1 , parent2  , layer;
    bool positive1 , positive2 , cur_positive;
};

struct Circuit{
    map<string , int> name_to_id_input;
    map<string , int> name_to_id_output;
    vector<string> input , output;
    vector<int> output_structure , output_gateNum , output_functional;
    vector<int> input_supp_size ,cir_cnf , fanin;
    int input_size , output_size , cnf_clauster_num , func_num;
    vector<set<int>> input_bus;
    vector<set<int>> output_bus;
    string filename;
    vector<int> output_id;
    vector<bool> output_possitive;
    vector<vector<int>> output_structure_port;
    vector<Node> func;
    map<int , int > bus_size;
    void get_filename(string file){filename = file;}
    void read_io();
    void read_bus(ifstream *inputfile);
    void read_circuit();
    void read_aig();
    void read_structure();
    void generate_cir_cnf();
    void get_input_supp_size();
};

struct Output{
    int functional ,structural ,fanins , id;
    vector<int> func_supp;
    string name;
    bool operator<(const Output &y);
};
