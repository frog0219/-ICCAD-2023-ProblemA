#include"input_solver.h"
#include <map>
#include <queue>
#include <cmath>
vector<vector<int>> input_result , output_result;
vector<int> buff_clause;
int max_ans = 0;
bool Incremental_SAT_based_Solver(Circuit cir1 , Circuit cir2 , stack<pair<int , int>> output_match_set ,ofstream &DeBug ,Timer start_time , string outputfile ,vector<vector<int>> &learn_clause){
    input_result = vector<vector<int>>(cir2.input_size , vector<int>((cir1.input_size + 1) * 2 , 0));
    output_result = vector<vector<int>>(cir2.output_size , vector<int>((cir1.output_size) * 2 , 0));
    vector<pair<int , int>> match_out;
    int cir2_port , cir1_port , xor_id  ;
    vector<int>add_cnf;
    bool debug = ((int)output_match_set.size() == cir2.output_size);
    CaDiCaL::Solver *mapping = new CaDiCaL::Solver;
    //init output miter
    xor_id = cir1.func_num + cir2.func_num;
    while(!output_match_set.empty()){
        xor_id++;
        pair<int , int> p = output_match_set.top();
        output_match_set.pop();
        match_out.emplace_back(p);
        cir2_port = p.first , cir1_port = p.second;

        output_result[cir2_port][2 * abs(cir1_port) + ((cir1_port >= 0)? 0 : 1)] = 1;
        
        if((cir1.output_possitive[abs(cir1_port)] == (cir1_port >= 0) == cir2.output_possitive[cir2_port]) ){
            // -c -> a xor b 
            add_cnf.emplace_back((cir1.output_id[abs(cir1_port)])) ,add_cnf.emplace_back((cir2.output_id[cir2_port] + cir1.func_num) * -1 ) ,add_cnf.emplace_back(xor_id) ,add_cnf.emplace_back(0);
            add_cnf.emplace_back((cir1.output_id[abs(cir1_port)]) * -1 ) ,add_cnf.emplace_back((cir2.output_id[cir2_port] + cir1.func_num)) ,add_cnf.emplace_back(xor_id),add_cnf.emplace_back(0);
            // c -> a nxor b
            add_cnf.emplace_back((cir1.output_id[abs(cir1_port)])) ,add_cnf.emplace_back((cir2.output_id[cir2_port] + cir1.func_num)) ,add_cnf.emplace_back(xor_id * -1) ,add_cnf.emplace_back(0);
            add_cnf.emplace_back((cir1.output_id[abs(cir1_port)]) * -1 ) ,add_cnf.emplace_back((cir2.output_id[cir2_port] + cir1.func_num)* -1 ) ,add_cnf.emplace_back(xor_id * -1) ,add_cnf.emplace_back(0);
        }
        else{
            // c -> a xor b 
            add_cnf.emplace_back((cir1.output_id[abs(cir1_port)])) ,add_cnf.emplace_back((cir2.output_id[cir2_port] + cir1.func_num) * -1 ) ,add_cnf.emplace_back(xor_id * -1) ,add_cnf.emplace_back(0);
            add_cnf.emplace_back((cir1.output_id[abs(cir1_port)]) * -1 ) ,add_cnf.emplace_back((cir2.output_id[cir2_port] + cir1.func_num)) ,add_cnf.emplace_back(xor_id * -1),add_cnf.emplace_back(0);
            // -c -> a nxor b
            add_cnf.emplace_back((cir1.output_id[abs(cir1_port)])) ,add_cnf.emplace_back((cir2.output_id[cir2_port] + cir1.func_num)) ,add_cnf.emplace_back(xor_id) ,add_cnf.emplace_back(0);
            add_cnf.emplace_back((cir1.output_id[abs(cir1_port)]) * -1 ) ,add_cnf.emplace_back((cir2.output_id[cir2_port] + cir1.func_num)* -1 ) ,add_cnf.emplace_back(xor_id) ,add_cnf.emplace_back(0);
        }
    }
    for(int i = cir1.func_num + cir2.func_num + 1;i <= xor_id ; i++) add_cnf.emplace_back(i);
    add_cnf.emplace_back(0);

    init_input_cnf(cir1 , cir2 , DeBug , mapping , learn_clause);
    buff_clause.clear();

    Timer total_time = Timer();
    //mapping->write_dimacs("input_match.cnf");
    while(true){
        for(int i = 0 ; i < match_out.size();i++){
            cout << "< " << match_out[i].first<<  " , " << match_out[i].second << " >\n";
        }
        CaDiCaL::Solver * miter = new CaDiCaL::Solver;

        if((float)total_time.GetElapsedMillseconds() / 1000.0 >= 100.0){
            delete miter;
            delete mapping;
            buff_clause.clear();
            match_out.clear();
            add_cnf.clear();
            return false;
        }

        //mapping->write_dimacs("input_match.cnf");
        //gen miter & input mapping
        if(!generate_matching_circuit_cnf(cir1 , cir2  , add_cnf , DeBug , mapping , miter)){
            delete miter;
            delete mapping;
            buff_clause.clear();
            match_out.clear();
            add_cnf.clear();
            return false;
        }
        //miter->write_dimacs("miter.cnf");
        //check miter
        //add learn clauster

        if(!modify_cnf(cir1 , cir2 , match_out, DeBug , mapping , miter)){
            if(match_out.size() > max_ans){
                write_ans(cir1 ,cir2 , outputfile);
                max_ans = match_out.size();
            }
            cout << "current time : " << (float)start_time.GetElapsedMillseconds()/1000.0 <<'\n';
            cout << "===================================\n";
            delete miter;
            break;
        }
        cout << "current time : " << (float)start_time.GetElapsedMillseconds()/1000.0 <<'\n';
        cout << "===================================\n";
        delete miter;
    }
    learn_clause.emplace_back(buff_clause);
    //cout << buff_clause.size();
    buff_clause.clear();
    match_out.clear();
    add_cnf.clear();
    delete mapping;
    return true;
}
bool modify_cnf(Circuit cir1 , Circuit cir2 , vector<pair<int , int>> match_out , ofstream &DeBug , CaDiCaL::Solver *mapping, CaDiCaL::Solver *miter){
    cout << "start modify\n";

    if(miter->solve() != 10 ) {
        cout << "get answer\n";
        return false;
    }
    vector<int> cir1_in(cir1.input_size) , cir2_in(cir2.input_size) , cir2_func , cir2_counter_output;
    set<int>  redundant_set;
    vector<int> output_match_result((int)match_out.size() , 0);
    int flag , cir1_in_id ,cir2_in_id;

    for(int i = 0 ; i < cir1.input_size ; i++){
        cir1_in[i] = miter->val(i + 1) > 0 ? 1 : 0; 
    }
    for(int i = 0; i < cir2.input_size ; i++){
        cir2_in[i] = miter->val(i + 1 + cir1.func_num) > 0 ? 1 : 0;
    }
    for(int i = 0 ; i < cir2.func_num ; i++){
        cir2_func.emplace_back(miter->val(i + 1 + cir1.func_num) > 0 ? 1 : 0);
    }
    for(int i = 0 ; i < (int)match_out.size() ;i++){
        output_match_result[i] = miter->val(i + 1 + cir1.func_num + cir2.func_num) > 0 ? 1 : 0;
    }
    for(int i = 0 ; i < cir2.output_id.size();i++){
        cir2_counter_output.emplace_back(cir2_func[cir2.output_id[i] - 1]);
    }


    for(int k = 0 ; k < (int)match_out.size();k++){
        if(output_match_result[k] == 1){
            redundant_set = gen_redomdam_set(cir2 , cir2_in ,  cir2_counter_output[match_out[k].first] , match_out[k].first);

            for(int i = 0 ; i < (int)cir2.output_structure_port[match_out[k].first].size() ;i++){

                if(redundant_set.find(cir2.output_structure_port[match_out[k].first][i]) != redundant_set.end())continue;
                cir2_in_id = cir2.output_structure_port[match_out[k].first][i] - 1;
                
                for(int j = 0 ; j < (int)cir1.output_structure_port[abs(match_out[k].second)].size() ;j++){

                    cir1_in_id = cir1.output_structure_port[abs(match_out[k].second)][j] - 1;
                    if(cir2_in[cir2_in_id] == cir1_in[cir1_in_id]){
                        
                        mapping->add(cir2_in_id * (cir1.input_size + 1 )* 2 + (cir1_in_id * 2 + 1)  +  1);
                        buff_clause.emplace_back(cir2_in_id * (cir1.input_size + 1 )* 2 + (cir1_in_id * 2 + 1)  +  1);
                    }
                    else{
                    
                        mapping->add(cir2_in_id * (cir1.input_size + 1 )* 2 + cir1_in_id * 2 + 1);
                        buff_clause.emplace_back(cir2_in_id * (cir1.input_size + 1 )* 2 + cir1_in_id * 2 + 1);
                    } 

                }
            }
            //constant
            if(cir1.output_structure_port[abs(match_out[k].second)].size() < cir2.output_structure_port[match_out[k].first].size()){

                for(int i = 0 ; i < (int)cir2.output_structure_port[match_out[k].first].size() ;i++){
                    if(redundant_set.find(cir2.output_structure_port[match_out[k].first][i]) != redundant_set.end())continue;

                    cir2_in_id = cir2.output_structure_port[match_out[k].first][i] - 1;

                    if(cir2_in[cir2_in_id] == 1) {
                        mapping->add( (cir2_in_id + 1)* (cir1.input_size + 1) * 2 - 1);
                        buff_clause.emplace_back( (cir2_in_id + 1)* (cir1.input_size + 1) * 2 - 1);
                    }
                    else {
                        mapping->add((cir2_in_id + 1)* (cir1.input_size + 1 )* 2);
                        buff_clause.emplace_back((cir2_in_id + 1)* (cir1.input_size + 1 )* 2);
                    }
                }

            }
            mapping->add(0);
            buff_clause.emplace_back(0);
            // for(int i = 0 ; i < buff_clause.size();i++){
            //     cout << buff_clause[i] << ' ';
            //     if(buff_clause[i] == 0) cout << '\n';
            // }
        }
    }
    cir1_in.clear() , cir2_in.clear() , cir2_func.clear() , cir2_counter_output.clear();
    
    redundant_set.clear();
    output_match_result.clear();
    cout << "end modify\n";
    return true;
}
void write_ans(Circuit cir1 , Circuit cir2 , string output_file_name){
    ofstream ans;
    bool ok;
    //int ans_num;
    ans.open(output_file_name);
    for(int i = 0 ; i < (int)output_result[0].size() ;i += 2){
        ok = true;
        for(int j = 0 ; j < (int)output_result.size();j++){
            if(output_result[j][i]){
                if(ok) {
                    ans << "OUTGROUP\n";
                    ans << "1 " << "+ "<< cir1.output[i / 2] << '\n';
                    ok = false;
                }
                ans << "2 " <<  "+ "<< cir2.output[j] << '\n';
            }
            if(output_result[j][i + 1]){
                if(ok) {
                    ans << "OUTGROUP\n";
                    ans << "1 " << "+ "<< cir1.output[i / 2] << '\n';
                    ok = false;
                }
                ans << "2 " <<  "- "<< cir2.output[j] << '\n';
            }
        }
        if(!ok) ans << "END\n";
    }
    for(int i = 0 ; i < (int)input_result[0].size() - 2;i += 2){
        ok = true;
        for(int j = 0 ; j < (int)input_result.size();j++){
            if(input_result[j][i]){
                if(ok) {
                    ans << "INGROUP\n";
                    ans << "1 " << "+ "<< cir1.input[i / 2] << '\n';
                    ok = false;
                }
                ans << "2 " <<  "+ "<< cir2.input[j] << '\n';
            }
            if(input_result[j][i + 1]){
                if(ok) {
                    ans << "INGROUP\n";
                    ans << "1 " << "+ "<< cir1.input[i / 2] << '\n';
                    ok = false;
                }
                ans << "2 " <<  "- "<< cir2.input[j] << '\n';
            }
        }
        if(!ok) ans << "END\n";
    }
    ans << "CONSTGROUP\n";
    for(int i = 0 ; i < (int)input_result.size();i++){
        if(input_result[i][input_result[0].size() - 2]){
            ans <<  "- "<< cir2.input[i] << '\n';
        }
        if(input_result[i][input_result[0].size() - 1]){
            ans <<  "+ "<< cir2.input[i] << '\n';
        }
        
    }
    ans << "END\n";
    ans.close();
}
bool generate_matching_circuit_cnf(Circuit cir1 , Circuit cir2  , vector<int> add_cnf , ofstream &DeBug , CaDiCaL::Solver *mapping , CaDiCaL::Solver *miter){
    cout << "start miter\n";
    if(mapping->solve() != 10) {
        cout << "no mapping solution\n";
        return false;
    }
    for(int i = 0 ; i < cir1.cir_cnf.size();i++)miter->add(cir1.cir_cnf[i]);
    for(int i = 0 ; i < cir2.cir_cnf.size();i++){
        if(cir2.cir_cnf[i] == 0)miter->add(cir2.cir_cnf[i]);
        else if (cir2.cir_cnf[i] > 0) miter->add(cir2.cir_cnf[i] + cir1.func_num);
        else miter->add(cir2.cir_cnf[i] - cir1.func_num);
    }
    //miter->write_dimacs("mix.cnf");
    for(int i = 0 ; i < add_cnf.size();i++){
        miter->add(add_cnf[i]);
        //cout << add_cnf[i] << ' ';
        //if(add_cnf[i] == 0) cout << '\n';
    }

    for(int i = 0 ; i < cir2.input_size;i++){
        for(int j = 0 ; j < ( cir1.input_size + 1) * 2; j++){
            input_result[i][j] = (mapping->val(i * ( cir1.input_size + 1) * 2 + j + 1) > 0 )? 1 : 0;
            //cout << input_result[i][j] << ' ';
            if(input_result[i][j]){
                if(j >= cir1.input_size * 2 ){
                    if(j % 2 == 0){
                        //cout << (i + cir1.func_num + 1)* -1 << " 0\n";
                        miter->add((i + cir1.func_num + 1)* -1 ) ,miter->add(0);
                    }
                    else {
                        //cout << (i + cir1.func_num + 1) << " 0\n";
                        miter->add((i + cir1.func_num + 1)) , miter->add(0);
                    }
                }
                else{
                    //cout << (i + cir1.func_num + 1) * -1  << ' ' << ((j / 2) + 1)*(j % 2 == 0 ? 1 : -1) << " 0\n";
                    //cout << (i + cir1.func_num + 1)  << ' ' << ((j / 2) + 1)*(j % 2 == 0 ? -1 : 1) << " 0\n";
                    miter->add((i + cir1.func_num + 1) * -1 ) ;
                    miter->add(((j / 2) + 1)*(j % 2 == 0 ? 1 : -1)) ;
                    miter->add(0);
                    miter->add((i + cir1.func_num + 1));
                    miter->add(((j / 2) + 1)*(j % 2 == 0 ? -1 : 1)) ;
                    miter->add(0);
                }
            }
        }
        //cout << '\n';
    }
    cout << "end miter\n";
    return true;
}
void init_input_cnf(Circuit cir1 , Circuit cir2 , ofstream &DeBug, CaDiCaL::Solver *mapping , vector<vector<int>> learn_clause){
    int permutation = cir1.input_bus.size() * cir2.input_bus.size() , bus_start = cir2.input_size * (cir1.input_size + 1)* 2 + 1 , flag;
    bool allow_constant = (cir1.input_size != cir2.input_size);

    if(!allow_constant){
        for(int i = 0 ; i < (int)cir2.input_size;i++){
            for(int j = 0 ; j < (int)cir1.input_size;j++){
                if(cir2.input_supp_size[i + 1] != cir1.input_supp_size[j + 1]){
                    mapping->add(-1 * (i * ((int)cir1.input_size + 1)* 2 + 2 * j + 1)) ,mapping->add(0);
                    mapping->add(-1 * (i * ((int)cir1.input_size + 1)* 2 + 2 * j + 2)) ,mapping->add(0);
                }
            }
            for(int j = cir1.input_size * 2 ; j <  (cir1.input_size + 1)* 2 ; j++){
                mapping->add(-1 * (i * ((int)cir1.input_size + 1)* 2 + j + 1)) , mapping->add(0);
            }
        }
    }
    //if no output pair's PI bind to 0

    //row can match only one
    int lg = ceil( log2(cir1.input_size + 1));
    for(int i = 0 ; i < (int)cir2.input_size ; i++){
        for(int j = 0 ; j <  (int)(cir1.input_size + 1)* 2 ; j++){
            mapping->add(i * (cir1.input_size + 1)* 2 + j + 1);
        }
        mapping->add(0);
        for(int j = 0 ; j <  (int)cir1.input_size + 1; j++){
            flag = j;
            for(int k = 0 ; k < lg; k++){
                mapping->add( (i * (cir1.input_size + 1)* 2 + j * 2 + 1 ) * - 1) , mapping->add( (k + bus_start ) * (flag % 2 == 1 ? 1 : -1 ) ) , mapping->add(0);
                mapping->add( (i * (cir1.input_size + 1)* 2 + j * 2 + 2 ) * - 1) , mapping->add(( k + bus_start ) * (flag % 2 == 1 ? 1 : -1 )) , mapping->add(0);
                flag = flag / 2;
            }
            //at most one
            mapping->add( (i * (cir1.input_size + 1)* 2 + j * 2 + 1 ) * - 1);
            mapping->add( (i * (cir1.input_size + 1)* 2 + j * 2 + 2 ) * - 1);
            mapping->add(0);
        }
        bus_start += lg;
    }
    
    // column at least one
    for(int i = 0 ; i < (int)cir1.input_size ; i++){
        for(int j = 0 ; j <  cir2.input_size ; j++){
            mapping->add(j * (cir1.input_size + 1)* 2 + i * 2 + 1);
            mapping->add(j * (cir1.input_size + 1)* 2 + i * 2 + 2);
        }
        mapping->add(0);
    }
    //bus matrix row at least one
    for(int i = 0 ; i < (int)cir2.input_bus.size();i++){
        for(int j = 0 ; j < (int)cir1.input_bus.size();j++){
            mapping->add(i * (int)cir1.input_bus.size() + j + bus_start);
        }
        mapping->add(0);
    }
    // bus matrix column at least one
    for(int i = 0 ; i < (int)cir1.input_bus.size();i++){
        for(int j = 0 ; j < (int)cir2.input_bus.size();j++){
            mapping->add(j * (int)cir1.input_bus.size() + i + bus_start);
        }
        mapping->add(0);
    }
    // bus information
    for(int i = 0 ; i < (int)cir2.input_bus.size() ; i++){
        for(int j = 0 ; j < (int)cir1.input_bus.size();j++){
            //for each bus match cir2->cir1
            for(int k : cir2.input_bus[i]){
                for(int m = 0 ; m < (int)cir1.input_size ; m++){
                    if(cir1.input_bus[j].find(m) != cir1.input_bus[j].end())continue;
                    mapping->add((i * (int)cir1.input_bus.size() + j + bus_start ) * -1);
                    mapping->add((k * ((int)cir1.input_size + 1 )* 2 + 2 * m + 1 ) * -1);
                    mapping->add(0);
                    mapping->add((i * (int)cir1.input_bus.size() + j + bus_start ) * -1);
                    mapping->add((k * ((int)cir1.input_size + 1 )* 2 + 2 * m + 2 ) * -1) ;
                    mapping->add(0);
                }
            }
            //for each bus match cir1 -> cir2
            for(int k : cir1.input_bus[j]){
                for(int m = 0 ; m < (int)cir2.input_size ; m++){
                    if(cir2.input_bus[i].find(m) != cir2.input_bus[i].end())continue;
                    mapping->add((i * (int)cir1.input_bus.size() + j + bus_start ) * -1);
                    mapping->add((m* ((int)cir1.input_size + 1 )* 2 + 2 * k + 1 ) * -1);
                    mapping->add(0);
                    mapping->add((i * (int)cir1.input_bus.size() + j + bus_start ) * -1);
                    mapping->add((m * ((int)cir1.input_size + 1 )* 2 + 2 * k + 2 ) * -1) ;
                    mapping->add(0);
                }
            }
        }
    }
    for(int i = 0 ; i < learn_clause.size();i++){
        for(int j = 0 ; j < learn_clause[i].size() ; j++)
            mapping->add(learn_clause[i][j]);
    }

}
set<int> gen_redomdam_set(Circuit cir2 , vector<int> counter_example , int output_ans , int id){
    set<int> redundant_set;
    vector<int> functional = cir2.output_structure_port[id];
    for(int i = 0 ; i < functional.size() ; i++){
        CaDiCaL::Solver * solver = new CaDiCaL::Solver;
        for(int i = 0 ; i < cir2.cir_cnf.size();i++){
            solver->add(cir2.cir_cnf[i]);
        }
        redundant_set.insert(functional[i]);
        for(int i = 0 ; i < functional.size() ; i++){
            if(redundant_set.find(functional[i]) == redundant_set.end()){
                solver->add(functional[i] * (counter_example[functional[i] - 1] == 1 ? 1 : -1)) , solver->add(0);
            }
        }
        solver->add(cir2.output_id[id] * (output_ans == 1 ? -1 : 1)) ,solver->add(0);
        if (solver->solve() == 10){
            redundant_set.erase(functional[i]);
        }
    }

    functional.clear();
    return redundant_set;
}