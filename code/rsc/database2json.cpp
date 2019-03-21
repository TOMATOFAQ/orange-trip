// #pragma GCC diagnostic error "-std=c++11"
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string> 
using namespace std;
using json = nlohmann::json;

json string2json(string s){

}

int main(){
    string line;
    ifstream in("database_origin.txt");
    while(getline(in,line)){
        cout << line << endl;
        string2json(line);
    }
    ofstream out("database.txt");
    return 0;
}
