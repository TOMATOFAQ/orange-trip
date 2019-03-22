#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
using json = nlohmann::json;

//全局的数据库
vector<json> D;
void ReadDatabase(){
    string line;
    ifstream infile;
    infile.open("database.txt");
    while(getline(infile,line)){
        json j = json::parse(line);
        D.push_back(j);
    }
}

int main(){
    ReadDatabase();
    
    for(vector<json>::iterator iter=D.begin();iter!=D.end();iter++){
        cout << (*iter) << endl;
        cout << (*iter)["from"] << endl;
    }
    return 0;
}