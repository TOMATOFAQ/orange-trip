#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
#include <vector>
#define MAX 99999
using namespace std;
using json = nlohmann::json;

// 两个等价的全局变量,一个以线性表表示，一个以邻接表表示
// 不可修改这两个全局变量！
vector<json> D;
void ReadDatabase() {
    string line;
    ifstream infile;
    infile.open("database.txt");
    while (getline(infile, line)) {
        json j = json::parse(line);
        D.push_back(j);
    }
}

json G = {};
void CreateDGraph() {  // 一个标准的邻接表表示的有向图
    for (json s : D) {
        string from = s["from"];
        G[from].push_back(s);
    }
}