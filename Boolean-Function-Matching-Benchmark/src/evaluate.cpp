//
// Created by grorge on 4/14/23.
//
#include "evaluate.h"
#include "utility.h"
#include <fstream>
#include <vector>
#include <sstream>
#include <cctype>
#include <ctime>
#include <cstdio>
#include <algorithm>
#include <regex>

extern Abc_Frame_t * pAbc;

struct CirIO{
    vector<string> input;
    vector<string> output;
};

struct Match{
    // the first value of input/output vector is cir1 and the other is cir2.
    vector< vector< pair<string, bool> > > input;
    vector< vector<pair< string, bool> > > output;
    vector<string> one;
    vector<string> zero;
};

CirIO readIO(ifstream* file){
    file->clear();
    file->seekg(0, std::ios::beg);
    CirIO re;
    string input;
    vector<string> portVector;
    bool noStop = true;
    do{
        getline(*file, input);
        if (input.find(";") != string::npos) noStop = false;
        stringstream ss;
        string inputName;
        ss.clear();
        ss << input;
        while (getline(ss, inputName, ',')){
            while(inputName.size() != 0 && !(isalpha(inputName.back()) || isdigit(inputName.back()) || inputName.back() == '_') )inputName.pop_back();
            while(inputName.size() != 0 && !(isalpha(inputName.front()) || isdigit(inputName.front()) || inputName.front() == '_') )inputName.erase(inputName.begin());
            if(inputName.size() == 0 || inputName.find("module") != string::npos || inputName.find("top") != string::npos)continue;
            portVector.push_back(inputName);
        }
    } while (noStop);
    while( getline(*file, input) ){
        bool readIO = false;
        if(input.find("input") != string::npos || input.find("output") != string::npos) {
            readIO = true;
            bool mod = input.find("input") != string::npos;
            stringstream ss;
            ss << input;
            string inputName;
            ss >> inputName; // remove input operator
            do{
                while (getline(ss, inputName, ',')){
                    if(inputName.find(";") != string::npos) {
                        readIO = false;
                    }
                    while(inputName.size() != 0 && !(isalpha(inputName.back()) || isdigit(inputName.back()) || inputName.back() == '_') )inputName.pop_back();
                    while(inputName.size() != 0 && !(isalpha(inputName.front()) || isdigit(inputName.front()) || inputName.front() == '_') )inputName.erase(inputName.begin());
                    if(inputName.size() == 0)continue;
                    if(mod){
                        re.input.push_back(inputName);
                    }else{
                        re.output.push_back(inputName);
                    }
                }
                if(readIO){
                    getline(*file, input);
                    ss.clear();
                    ss << input;
                }
            } while (readIO);
        }
    }
    for(auto i : portVector){
        if((find(re.input.begin(), re.input.end(), i) == re.input.end()) && (find(re.output.begin(), re.output.end(), i) == re.output.end())){
            re.output.push_back(i);
        }
    }
    return re;
}

Match readOutput(ifstream* file){
    file->clear();
    file->seekg(0, std::ios::beg);
    Match re;
    string input;
    while( getline(*file, input) ){
        if(input =="INGROUP" || input == "OUTGROUP"){
            bool mod = input == "INGROUP";
            string port;
            vector<pair<string, bool> > tmp = {pair<string, bool>("", 0)};
            bool check = false;
            while(getline(*file, port)){
                if(port == "END")break;
                stringstream ss;
                ss << port;
                int number;
                char isPositive;
                string name;
                ss >> number >> isPositive >> name;
                if(number == 1){
                    if(check){
                        cout << "There are multiple cir1 " << (mod ? "input" : "output") << " port(" << name << ") in a group." << endl;
                        exit(1);
                    }
                    check = true;
                    tmp[0].first = name;
                    tmp[0].second = isPositive == '+';
                }else{
                    tmp.push_back(pair<string, bool>(name, isPositive == '+') );
                }
            }
            if(!check){
                cout << "Can not find cir1 port." << endl;
                exit(1);
            }
            if(tmp.size() < 2){
                cout << "Can not find cir2 port." << endl;
                exit(1);
            }
            if(mod){
                re.input.push_back(tmp);
            }else{
                re.output.push_back(tmp);
            }
        }else if(input == "CONSTGROUP"){
            string port;
            while(getline(*file, port)){
                if(port == "END")break;
                stringstream ss;
                ss << port;
                char isPositive;
                string name;
                ss >> isPositive >> name;
                if(isPositive == '+'){
                    re.zero.push_back(name);
                }else{
                    re.one.push_back(name);
                }
            }
        }else if(input.size() != 0){
            cout << "Can not read output file." << endl;
            exit(1);
        }
    }
    return re;
}
void checkOutput(Match match){
    set<string> cir2, cir1;
    for(auto groups : match.input ){
        if(cir1.find(groups[0].first) != cir1.end()){
            cout << "cir1 port "<< groups[0].first << " is matched to multiple value." << endl;
            exit(1);
        }else{
            cir1.insert(groups[0].first);
        }
        for(int i = 1 ; i < static_cast<int>(groups.size()) ; i ++){
            if(cir2.find(groups[i].first) != cir2.end()){
                cout << "cir2 port "<< groups[i].first << " is matched to multiple value." << endl;
                exit(1);
            }else{
                cir2.insert(groups[i].first);
            }
        }
    }
    for(auto it : match.one){
        if(cir2.find(it) != cir2.end()){
            cout << "cir2 port "<< it << " is matched to multiple value." << endl;
            exit(1);
        }else{
            cir2.insert(it);
        }
    }
    for(auto it : match.zero){
        if(cir2.find(it) != cir2.end()){
            cout << "cir2 port "<< it << " is matched to multiple value." << endl;
            exit(1);
        }else{
            cir2.insert(it);
        }
    }
    return;
}
void writeNewCir(
        CirIO cirIO1, CirIO cirIO2, Match match,
        string cir1Path, string cir2Path, ifstream* cirFile1, ifstream* cirFile2){
    cirFile1->clear();
    cirFile1->seekg(0, std::ios::beg);
    cirFile2->clear();
    cirFile2->seekg(0, std::ios::beg);
    ofstream newCirFile1, newCirFile2;
    newCirFile1.open(cir1Path, ios::out);
    newCirFile2.open(cir2Path, ios::out);
    set<string> matchInput[2], matchOutput[2];
    if(!newCirFile1.is_open()){
        cout << "Can't write file:" << cir1Path << endl;
        exit(1);
    }
    if(!newCirFile2.is_open()){
        cout << "Can't write file:" << cir1Path << endl;
        exit(1);
    }
    newCirFile1 << "module top();\n";
    for(int i = 0 ; i < static_cast<int>(cirIO1.input.size()) ; i++){
        newCirFile1 << "wire " << cirIO1.input[i] << ";\n";
    }
    for(int i = 0 ; i < static_cast<int>(cirIO1.output.size()) ; i++){
        newCirFile1 << "wire " << cirIO1.output[i] << ";\n";
    }
    newCirFile2 << "module top2();\n";
    for(int i = 0 ; i < static_cast<int>(cirIO2.input.size()) ; i++){
        newCirFile2 << "wire " << cirIO2.input[i] << ";\n";
    }
    for(int i = 0 ; i < static_cast<int>(cirIO1.output.size()) ; i++){
        newCirFile2 << "wire " << cirIO2.output[i] << ";\n";
    }
    int cnt = 0;
    vector<string> setVector[2];
    for(int i = 0 ; i < static_cast<int>(match.input.size()) ; i++){
        newCirFile1 << "input matchInput" << cnt << ";\n";
        newCirFile2 << "input matchInput" << cnt << ";\n";
        matchInput[0].insert(match.input[i][0].first);
        if(match.input[i][0].second){
            setVector[0].push_back("assign " + match.input[i][0].first + " = matchInput" + to_string(cnt) + ";\n");
        }else{
            setVector[0].push_back( "not( " + match.input[i][0].first + ", " + "matchInput" + to_string(cnt) + " );\n");
        }
        for(int q = 1 ; q < static_cast<int>(match.input[i].size()) ; q++){
            matchInput[1].insert(match.input[i][q].first);
            if(match.input[i][q].second){
                setVector[1].push_back( "assign " + match.input[i][q].first + " = matchInput" + to_string(cnt) + ";\n");
            }else{
                setVector[1].push_back("not( " + match.input[i][q].first + ", " + "matchInput" + to_string(cnt) + " );\n");
            }
        }
        cnt++;
    }

    for(int i = 0 ; i < static_cast<int>(match.output.size()) ; i++){
        for(int q = 1 ; q < static_cast<int>(match.output[i].size()) ; q++){
            newCirFile1 << "output matchOutput" << cnt << ";\n";
            newCirFile2 << "output matchOutput" << cnt << ";\n";
            matchOutput[0].insert(match.output[i][0].first);
            if(match.output[i][0].second){
                setVector[0].push_back( "assign matchOutput" + to_string(cnt) + " = " + match.output[i][0].first + ";\n");
            }else{
                setVector[0].push_back("not( matchOutput" + to_string(cnt) + ", " + match.output[i][0].first + " );\n");
            }
            matchOutput[1].insert(match.output[i][q].first);
            if(match.output[i][q].second){
                setVector[1].push_back("assign matchOutput" + to_string(cnt) + " = " + match.output[i][q].first + ";\n");
            }else{
                setVector[1].push_back("not( matchOutput" + to_string(cnt) + ", " + match.output[i][q].first + " );\n");
            }
            cnt++;
        }
    }
    for(int i = 0 ; i < static_cast<int>(match.one.size()) ; i++){
        matchInput[1].insert(match.one[i]);
    }
    for(int i = 0 ; i < static_cast<int>(match.zero.size()) ; i++){
        matchInput[1].insert(match.zero[i]);
    }
    for(int i = 0 ; i < static_cast<int>(cirIO1.input.size()) ; i++){
        auto it = matchInput[0].find(cirIO1.input[i]);
        if(it == matchInput[0].end()){
            newCirFile1 << "input unMatchOutput" << cnt << ";\n";
            newCirFile2 << "input unMatchOutput" << cnt << ";\n";
            setVector[0].push_back("assign " + cirIO1.input[i] + " = " + "unMatchOutput" + to_string(cnt) + ";\n");
            cnt++;
        }else{
            matchInput[0].erase(it);
        }
    }
    for(int i = 0 ; i < static_cast<int>(cirIO2.input.size()) ; i++){
        auto it = matchInput[1].find(cirIO2.input[i]);
        if( it == matchInput[1].end()){
            newCirFile1 << "input unMatchOutput" << cnt << ";\n";
            newCirFile2 << "input unMatchOutput" << cnt << ";\n";
            setVector[1].push_back("assign " + cirIO2.input[i] + " = " + "unMatchOutput" + to_string(cnt) + ";\n");
            cnt++;
        }else{
            matchInput[1].erase(it);
        }
    }
    if(matchInput[0].size() != 0){
        cout << "There is a input port(" << *matchInput[0].begin() << ") not exist circuit 1 but occur in output file." << endl;
        exit(1);
    }
    if(matchInput[1].size() != 0){
        cout << "There is a input port(" << *matchInput[1].begin() << ") not exist circuit 2 but occur in output file." << endl;
        exit(1);
    }
    for(int i = 0 ; i < static_cast<int>(cirIO1.output.size()) ; i++){
        auto it = matchOutput[0].find(cirIO1.output[i]);
        if(it != matchOutput[0].end()){
            matchOutput[0].erase(it);
        }
    }
    for(int i = 0 ; i < static_cast<int>(cirIO2.output.size()) ; i++){
        auto it = matchOutput[1].find(cirIO2.output[i]);
        if(it != matchOutput[1].end()){
            matchOutput[1].erase(it);
        }
    }
    if(matchOutput[0].size() != 0){
        cout << "There is a output port(" << *matchOutput[0].begin() << ") not exist circuit 1 but occur in output file." << endl;
        exit(1);
    }
    if(matchOutput[1].size() != 0){
        cout << "There is a output port(" << *matchOutput[1].begin() << ") not exist circuit 2 but occur in output file." << endl;
        exit(1);
    }

    for(int i = 0 ; i < static_cast<int>(setVector[0].size()) ; i++){
        newCirFile1 << setVector[0][i];
    }
    for(int i = 0 ; i < static_cast<int>(setVector[1].size()) ; i++){
        newCirFile2 << setVector[1][i];
    }
    for(int i = 0 ; i < static_cast<int>(match.one.size()) ; i++){
        newCirFile2 << "assign " << match.one[i] << " = 1'b1;\n";
    }
    for(int i = 0 ; i < static_cast<int>(match.zero.size()) ; i++){
        newCirFile2 << "assign " << match.zero[i] << " = 1'b0;\n";
    }
    string input;
    while (getline(*cirFile1, input)){
        if(!(input.find("input") != string::npos || input.find("output") != string::npos || input.find("module ") != string::npos)){
            newCirFile1 << input << '\n';
        }else{
            while (input.find(";") == string::npos){
                getline(*cirFile1, input);
            }
        }
    }
    while (getline(*cirFile2, input)){
        if(!(input.find("input") != string::npos || input.find("output") != string::npos || input.find("module ") != string::npos)){
            newCirFile2 << input << '\n';
        }else{
            while (input.find(";") == string::npos){
                getline(*cirFile2, input);
            }
        }
    }
    newCirFile1.close();
    newCirFile2.close();
    return;
}
bool readFileEquivalent(string path){
    bool equivalent = false;
    ifstream readFile(path, ios::in);
    if(readFile.is_open()){
        string result;
        while (getline(readFile, result)){
            if(result.find("Networks are equivalent") != string::npos){
                equivalent = true;
            }
        }
        readFile.close();
    }else{
        cout << "Can't read writeFile:" << path << endl;
        exit(1);
    }
    ofstream writeFile(path, std::ios::trunc);
    if (writeFile.is_open()) {
        writeFile.close();
    } else {
        cout << "Can't clean writeFile:" << path << endl;
        exit(1);
    }
    return equivalent;
}
void removeGateNames(const std::string& filePath) {
    std::ifstream inputFile(filePath);
    std::string outputFilePath = filePath + ".modified";
    std::ofstream outputFile(outputFilePath);

    std::regex gateNameRegex("(\\bnot\\b|\\band\\b|\\bnand\\b|\\bor\\b|\\bxor\\b|\\bxnor\\b|\\bbuf\\b|\\bnor\\b)\\s*(\\w+)");

    for (std::string line; std::getline(inputFile, line); ) {
        std::string modifiedLine = std::regex_replace(line, gateNameRegex, "$1");

        outputFile << modifiedLine << std::endl;
    }

    inputFile.close();
    outputFile.close();
    rename(outputFilePath.c_str(), filePath.c_str());
}
bool cecTest(string cir1, string cir2){
    removeGateNames(cir1);
    removeGateNames(cir2);
    string resultPath = "output-" + getNowTime() + ".txt";
    cout.flush();
    FILE *saveStdout = stdout;
    bool finalResult = true;
    stdout = fopen(resultPath.c_str(), "w");
    if (stdout != NULL) {
        string command;
        // remove redundant node
//        command = "read " + cir1 + "; balance; cleanup;cec -sn; write_verilog " + cir1;
//        if (Cmd_CommandExecute(pAbc, command.c_str())){
//            cout << "Cannot execute command \"" << command << "\".\n";
//            exit(1);
//        }
//        fflush(stdout);
//        finalResult = readFileEquivalent(resultPath) && finalResult;
//        command = "read " + cir2 + "; balance; cleanup;cec -sn; write_verilog " + cir2;
//        if (Cmd_CommandExecute(pAbc, command.c_str())){
//            cout << "Cannot execute command \"" << command << "\".\n";
//            exit(1);
//        }
//        fflush(stdout);
//        finalResult = readFileEquivalent(resultPath) && finalResult;

        command = "cec " + cir1 + " " + cir2;
        if (Cmd_CommandExecute(pAbc, command.c_str())){
            cout << "Cannot execute command \"" << command << "\".\n";
            exit(1);
        }
        fflush(stdout);
        finalResult = readFileEquivalent(resultPath) && finalResult;
        fclose(stdout);
        stdout = saveStdout;
    } else {
        cout << "Can't write file:" << resultPath << endl;
        exit(1);
    }
    if(std::remove(cir1.c_str())){
        cout << "Can't delete fie:" << cir1 << endl;
    }
    if(std::remove(cir2.c_str())){
        cout << "Can't delete fie:" << cir2 << endl;
    }
    if(std::remove(resultPath.c_str())){
        cout << "Can't delete fie:" << resultPath << endl;
    }
    return finalResult;
}

Report judge(string outPath, string cirPath1, string cirPath2){
    Report report{cirPath1,cirPath2,time(nullptr),true, 0};
    ifstream* outputFile = readFile(outPath);
    ifstream* cirFile1 = readFile(cirPath1);
    ifstream* cirFile2 = readFile(cirPath2);

    CirIO cirIO1 = readIO(cirFile1);
    CirIO cirIO2 = readIO(cirFile2);
    Match match = readOutput(outputFile);
    int max_pts = cirIO1.output.size() + cirIO2.output.size();
    int points = 0;
    for(auto it : match.output){
        points += it.size();
    }
    string newCir1Path = "nCir1-" + getNowTime() + ".v";
    string newCir2Path = "nCir2-" + getNowTime() + ".v";
    writeNewCir(cirIO1, cirIO2, match, newCir1Path, newCir2Path, cirFile1, cirFile2);
    checkOutput(match);
    bool equivalent = cecTest(newCir1Path, newCir2Path);
    report.passed = equivalent;
    if(report.passed)
        report.score = static_cast<double>(points) / max_pts * 100;
    outputFile->close();
    cirFile1->close();
    cirFile2->close();
    delete outputFile;
    delete cirFile1;
    delete cirFile2;
    return report;
}