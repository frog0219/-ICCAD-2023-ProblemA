#include"output_solver.h"
using namespace std;
bool optimal = false, timeout = false, projection = false;
pair<int , int> new_pair;
int cur_group = 0, cur_index = 0;
void output_solver(Circuit cir1 , Circuit cir2 , Timer start_time , string outputfile){
    pair<vector<vector<Output>> , vector<vector<Output>>> G;
    vector<vector<Output>> group_cir1 , group_cir2;
    vector<vector<int>> output_result(cir2.output_size , vector<int>(cir1.output_size * 2 , -1));
    stack<pair<int , int>> output_match_set;
    vector<int> cir2_match_group_id(cir2.output_size , 0);
    vector<bool> cir2_match_possitive(cir2.output_size , true) , cir1_use(cir1.output_size , false);
    ofstream DeBug;
    vector<vector<int>> learn_clause;
    int now_match;
    G = init_output_group(cir1 , cir2);
    group_cir1 = G.first , group_cir2 = G.second;
    for(int i = 0 ; i < (int)group_cir1.size();i++){
        cout <<"cir1 group "<< i << ": ";
        for(int j = 0 ; j < group_cir1[i].size();j++){
            cout << group_cir1[i][j].name << ", id : " << group_cir1[i][j].id << " | ";
        }
        cout <<'\n';
    }
    for(int i = 0 ; i < group_cir2.size();i++){
         cout << "cir2 group "<< i  << ": ";
        for(int j = 0 ; j < (int)group_cir2[i].size();j++){
            cout << group_cir2[i][j].name <<", id : " << group_cir2[i][j].id << " | ";
        }
        cout <<'\n';
    }
    cout << "cir1 input bus : \n";
    for(int i = 0 ; i < cir1.input_bus.size();i++){
        cout << "bus " << i  << " : ";
        for(int j : cir1.input_bus[i]) cout << j << "  ";
        cout << "\n";
    }
    cout << "cir2 input bus\n";
    for(int i = 0 ; i < cir2.input_bus.size();i++){
        cout << "bus " << i  << " : ";
        for(int j : cir2.input_bus[i]) cout << j << "  ";
        cout << "\n";
    }
    while(!optimal && !timeout){
        // find new pair
        if(find_new_pair_group(group_cir1 , group_cir2 ,cir2_match_group_id ,cir2_match_possitive ,cir1_use)){
            output_match_set.push(new_pair);

            if(!Incremental_SAT_based_Solver(cir1 ,cir2 , output_match_set ,DeBug , start_time ,outputfile , learn_clause)){
                output_match_set.pop();
                now_match = cir2_match_group_id[group_cir2[cur_group][cur_index].id];
                cir1_use[group_cir1[cur_group][now_match].id] = false;
                if(cir2_match_possitive[group_cir2[cur_group][cur_index].id])cir2_match_group_id[group_cir2[cur_group][cur_index].id]++;
            }
            else{
                if(cir2_match_possitive[group_cir2[cur_group][cur_index].id])cir2_match_group_id[group_cir2[cur_group][cur_index].id]++;
                cur_index++;
                if(cur_index >= (int)group_cir2[cur_group].size())cur_group++ , cur_index = 0;
            }
        }
        else{
            cout << "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB\n";
            cir1_use[abs(output_match_set.top().second)] = false;
            output_match_set.pop();
            cir2_match_group_id[group_cir2[cur_group][cur_index].id] = 0;
            cur_index--;
            if(cur_index < 0)cur_group-- , cur_index = group_cir2[cur_group].size() - 1;
            if(cur_group < 0)break;
            learn_clause.pop_back();
        }
        timeout = ((float)start_time.GetElapsedMillseconds() /1000.0 >= 3500.0);
        optimal = ((int)output_match_set.size() == cir2.output_size);
    }
}
pair<vector<vector<Output>> , vector<vector<Output>>> init_output_group(Circuit cir1 , Circuit cir2){
    vector<Output> o1 , o2;
    for(int i = 0 ; i < cir1.output_size ; i++){
        Output flag;
        flag.structural = cir1.output_functional[i];
        flag.functional = cir1.output_structure[i];
        flag.fanins = cir1.fanin[i];
        flag.id = i , flag.name = cir1.output[i];
        o1.emplace_back(flag);
    }
    for(int i = 0 ; i < cir2.output_size ; i++){
        Output flag;
        flag.structural = cir2.output_functional[i];
        flag.functional = cir2.output_structure[i];
        flag.fanins = cir2.fanin[i];
        flag.id = i , flag.name = cir2.output[i];
        o2.emplace_back(flag);
    }
    cout << "end\n";
    sort(o1.begin(), o1.end());
    //for(int i = 0 ; i < o2.size() ;i++)cout << i << " " <<o2[i].id << '\n';
    sort(o2.begin(), o2.end());
    //for(int i = 0 ; i < o2.size() ;i++)cout << i << " " <<o2[i].id << '\n';
    vector<vector<Output>> flag_1 , flag_2;
    vector<vector<Output>> group_1 , group_2;
    vector<Output> flag ;
    vector<Output> buff1 , buff2;
    int index = 0;
    flag_1.emplace_back(flag) , flag_2.emplace_back(flag);
    for(int i = 0 ; i < o2.size() ;i++){
        flag_1[index].emplace_back(o1[i]);
        flag_2[index].emplace_back(o2[i]);
        if(i < o1.size() - 1 && o1[i + 1].functional > o2[i].functional){
            flag_1.emplace_back(flag);
            flag_2.emplace_back(flag);
            index++;
        }
    }
    cout << "end\n";
    for(int i = 0 ; i < flag_1.size();i++){
        if(flag_1[i][0].functional == 1){
            buff1 = flag_1[i];
        }
        else group_1.emplace_back(flag_1[i]) ;

        if(flag_2[i][0].functional == 1){
            buff2 = flag_2[i];
        }
        else group_2.emplace_back(flag_2[i]);
    }
    if(buff1.size() > 0)group_1.emplace_back(buff1);
    if(buff2.size() > 0)group_2.emplace_back(buff2);
    return pair<vector<vector<Output>> , vector<vector<Output>>>(group_1 , group_2);
}
bool find_new_pair_group(vector<vector<Output>> group_cir1 , vector<vector<Output>> group_cir2 , vector<int> &cir2_match_group_id , vector<bool> &cir2_match_possitive ,vector<bool> &cir1_use){
    if(cur_group >= (int)group_cir2.size()) {
        return false;
    }
    Output cur = group_cir2[cur_group][cur_index];
    int now_match = cir2_match_group_id[cur.id];
    int limit_id_group = group_cir1[cur_group].size();
    if(cir2_match_possitive[cur.id]){
        while(cir1_use[group_cir1[cur_group][now_match].id] && now_match < limit_id_group) now_match++;

        if(now_match == limit_id_group){
            return false;
        }
        new_pair = pair<int , int>(cur.id , group_cir1[cur_group][now_match].id);
        cir2_match_group_id[cur.id] = now_match;
        cir2_match_possitive[cur.id] = false;
        cir1_use[group_cir1[cur_group][now_match].id] = true;
    }
    else{
        new_pair = pair<int , int>(cur.id , group_cir1[cur_group][now_match].id * -1);
        cir1_use[group_cir1[cur_group][now_match].id] = true;
        cir2_match_possitive[cur.id] = true;
    }
    return true;
}