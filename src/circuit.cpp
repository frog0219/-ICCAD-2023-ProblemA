#include "circuit.h"
using namespace std;
void Circuit::read_circuit(){
    Abc_Start();
    Abc_Frame_t * pAbc = Abc_FrameGetGlobalFrame();
    read_verilog(pAbc , filename);
    transform2aig(pAbc);
    write_aiger(pAbc ,"outputfile.aig");
    transform2ascii("outputfile.aig" , "outputfile.aag");
    print_io(pAbc);
    print_suppset(pAbc);
    print_func_suppset(pAbc);
    Abc_Stop();
    read_io();
    read_aig();
    read_structure();
    get_input_supp_size();
    Abc_Stop();
    generate_cir_cnf();
}
void Circuit::read_io(){
    ifstream io ("io.txt");
    string port;
    if (io.is_open()) {
        io >> this->input_size;
        for (int i = 1 ; i <= input_size ;i++){
            io >> port;
            input.push_back(port);
            name_to_id_input.insert(pair<string , int>(port , i));
        }
        io >> this->output_size;
        for (int i = 0 ; i < output_size ;i++){
            io >> port;
            output.push_back(port);
            name_to_id_output.insert(pair<string , int>(port , i));
        }
    }
    io.close();
}
void Circuit::read_bus(ifstream *inputfile){
    if ((*inputfile).is_open()) {
        int single_bus_size , num_bus;
        string port;
        (*inputfile) >> num_bus;
        for(int i = 0 ; i < num_bus ;i++){
            (*inputfile) >> single_bus_size;
            vector<string> single_bus(single_bus_size);
            set<int> id_bus;
            for(int j = 0 ; j < single_bus_size ; j++){
                (*inputfile) >> port;
                single_bus[j] = port;
                id_bus.insert(name_to_id_input.find(single_bus[j])->second - 1);
            }
            if(name_to_id_input.find(single_bus[0]) != name_to_id_input.end())input_bus.emplace_back(id_bus);
            else output_bus.emplace_back(id_bus);
        }
    }
}
void Circuit::read_aig(){
    cout << "start read_aig\n";
    ifstream aig;
    int total_gate , in_size , unknow, out_size , num ,port;
    bool in1_p ,in2_p;
    string format;
    queue<Node> Q;
    vector<Node> additional;
    Node cur;
    int out , in1 ,in2 ,next_func_id;
    aig.open("outputfile.aag");
    if (aig.is_open()) {
        aig >> format >> total_gate >> in_size >> unknow >> out_size >> num;
        func = vector<Node>(total_gate + 1);
        next_func_id = total_gate + 1;
        func[0] = Node(0, -1 , -1 , true , true , true , 0);
        for(int i = 1 ; i <= in_size ; i++){
            aig >> port;
            func[i] = Node(i, -1 , -1 , true , true , true , 0);
        }
        for(int i = 0 ; i < out_size ; i++){
            aig >> port;
            this->output_id.emplace_back(port / 2);
            if(port >= 2)this->output_possitive.emplace_back((port % 2 == 0));
            else this->output_possitive.emplace_back((port % 2 == 1));  
        }
        for(int i = 0 ; i < num ; i++){
            aig >> out >> in1 >> in2;
            in1_p = ((in1 >= 2) ? (in1 % 2 == 0) : (in1 % 2 == 1));
            in2_p = ((in2 >= 2) ? (in2 % 2 == 0) : (in2 % 2 == 1));
            func[out / 2] = Node( out / 2 , in1 / 2 , in2 / 2 , in1_p , in2_p , (out % 2 == 0) , 1);
        }
    }
    aig.close();
    func_num = func.size() - 1;
    cout << "finish read_aig\n";
}
void Circuit::read_structure(){
    ifstream structfile("struct.txt" ,  ifstream::in) , functional;
    string str;
    int NodeNum , SuppNum , funcNum;
    if (structfile.is_open()) {
        for (int i = 0 ; i < output_size ; i++) {
            structfile >> str >> NodeNum >> SuppNum;
            if(SuppNum == 0 && output_id[i] > 1)SuppNum++;
            output_gateNum.emplace_back(NodeNum);
            output_structure.emplace_back(SuppNum);
        }
    }
    structfile.close();
    functional.open("functional.txt");
    if (functional.is_open()) {
        for(int i = 0 ; i < output_size ; i++) {
            functional >> str >> funcNum;
            if(funcNum == 0)funcNum++;
            output_functional.emplace_back(funcNum);
        }
        for(int i = 0 ; i < output_size ; i++){
            vector<int> result;
            for(int j = 0 ; j < output_functional[i] ; j++) {
                functional >> str;
                auto flag = name_to_id_input.find(str);
                if(flag != name_to_id_input.end())result.emplace_back(flag->second);
            }
            output_structure_port.emplace_back(result);
        }
    }
    functional.close();
}
void Circuit::generate_cir_cnf(){
    cout << "gen cnf\n";
    vector<int> a(3 , 0) , b(3 , 0) ,c(4 , 0);
    for(int i = 1 ; i <= func_num ; i++){
        if(func[i].parent1 == -1)continue;
        else{
            a[0] = -1 * (func[i].id), a[1] = (func[i].positive1 ? 1 : -1) * (func[i].parent1);
            b[0] = -1 * (func[i].id), b[1] = (func[i].positive2 ? 1 : -1) * (func[i].parent2);
            c[0] =  (func[i].id ) , c[1] = (func[i].positive1 ? -1 : 1) * (func[i].parent1) , c[2] = (func[i].positive2 ? -1 : 1) * (func[i].parent2);
            if(func[i].parent1 != 0 && func[i].parent2 != 0){
                cir_cnf.emplace_back(a[0]) , cir_cnf.emplace_back(a[1]) , cir_cnf.emplace_back(0);
                cir_cnf.emplace_back(b[0]) , cir_cnf.emplace_back(b[1]) , cir_cnf.emplace_back(0) ;
                cir_cnf.emplace_back(c[0]) , cir_cnf.emplace_back(c[1])  ,cir_cnf.emplace_back(c[2]) , cir_cnf.emplace_back(0);
            }
            else if(func[i].parent1 == 0 && func[i].parent2 != 0){
                if(func[i].positive1){
                    cir_cnf.emplace_back(b[0]) , cir_cnf.emplace_back(b[1]) , cir_cnf.emplace_back(0);
                    cir_cnf.emplace_back(c[0])  ,cir_cnf.emplace_back(c[2]) , cir_cnf.emplace_back(0);
                }
                else{
                    cir_cnf.emplace_back(a[0]) , cir_cnf.emplace_back(0);
                }
            }
            else if(func[i].parent1 != 0 && func[i].parent2 == 0){
                if(func[i].positive2){
                    cir_cnf.emplace_back(a[0]) , cir_cnf.emplace_back(a[1]) , cir_cnf.emplace_back(0);
                    cir_cnf.emplace_back(c[0])  ,cir_cnf.emplace_back(c[1]) , cir_cnf.emplace_back(0);
                }
                else{
                    cir_cnf.emplace_back(b[0]) , cir_cnf.emplace_back(0);
                }
            }
            else{
                if(func[i].positive1 && func[i].positive2){
                    cir_cnf.emplace_back(c[0]) , cir_cnf.emplace_back(0);
                }
                else{
                    cir_cnf.emplace_back(a[0]) , cir_cnf.emplace_back(0);
                }
            }
        }
    }
    // queue<Node> Q;
    // vector<int> a(3 , 0) , b(3 , 0) ,c(4 , 0);
    // bool is_write[func.size()] = {false};
    // cir_cnf.clear();
    // for(int i = 0 ; i < output_id.size(); i++){
    //     Q.push(func[output_id[i]]);
    // }
    // while(!Q.empty()){
    //     Node cur = Q.front();
    //     Q.pop();
    //     if(cur.layer == 0) continue;
    //     if(!is_write[cur.id]){
    //         if(func[cur.parent1].layer > 0) Q.push(func[cur.parent1]);
    //         if(func[cur.parent2].layer > 0) Q.push(func[cur.parent2]);
    //         //normal 
    //         if(cur.parent1 == cur.parent2){
    //             if(cur.parent1 > 0){
    //                 cir_cnf.emplace_back(cur.id * -1) , cir_cnf.emplace_back(cur.parent1) , cir_cnf.emplace_back(0);
    //                 cir_cnf.emplace_back(cur.id) , cir_cnf.emplace_back(cur.parent1 * -1) , cir_cnf.emplace_back(0);
    //             }
    //             else{
    //                 cir_cnf.emplace_back(cur.id * (cur.positive1 ? -1 : 1)) , cir_cnf.emplace_back(0);
    //             }
    //         }
    //         else{
    //             a[0] = -1 * (cur.id), a[1] = (cur.positive1 ? 1 : -1) * (cur.parent1 );
    //             b[0] = -1 * (cur.id), b[1] = (cur.positive2 ? 1 : -1) * (cur.parent2 );
    //             c[0] = 1 * (cur.id) , c[1] = (cur.positive1 ? -1 : 1) * (cur.parent1 ) , c[2] = (cur.positive2 ? -1 : 1) * (cur.parent2);
    //             cir_cnf.emplace_back(a[0]) , cir_cnf.emplace_back(a[1]) , cir_cnf.emplace_back(a[2]);
    //             cir_cnf.emplace_back(b[0]) ,cir_cnf.emplace_back(b[1])  ,cir_cnf.emplace_back(b[2]) ;
    //             cir_cnf.emplace_back(c[0]) ,cir_cnf.emplace_back(c[1])  ,cir_cnf.emplace_back(c[2]) ,cir_cnf.emplace_back(c[3]); 
    //         }
    //         is_write[cur.id] = true;
    //     }
    // }
}
void Circuit::get_input_supp_size(){
    queue<Node> Q;
    Node cur;
    input_supp_size = vector<int>(input_size + 1, 0);
    fanin =  vector<int>(output_id.size(), 0);
    for(int i = 0 ; i < output_id.size(); i++){
        bool is_count[input_size + 1] = {false};
        Q.push(func[output_id[i]]);
        while(!Q.empty()){
            cur = Q.front();
            Q.pop();
            if(cur.parent1 >= 0) Q.push(func[cur.parent1]);
            if(cur.parent2 >= 0) Q.push(func[cur.parent2]);
            if(cur.layer == 0 && !is_count[cur.id])input_supp_size[cur.id]++ , is_count[cur.id] = true;
            if(cur.layer != 0) fanin[i]++;
        }
    }
}
bool Output :: operator<(const Output &y){
        if(functional == y.functional){
            return structural < y.structural;
        }
        return functional < y.functional;
}