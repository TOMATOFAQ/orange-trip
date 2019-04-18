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
static vector<json> D;
static void ReadDatabase() {
    ifstream ifstream_database("database.json");
    json temp;
    ifstream_database >> temp;

    for (json j : temp) {
        D.push_back(j);
    }
}

static json G = {};
static void CreateDGraph() {  // 一个标准的邻接表表示的有向图
    for (json s : D) {
        string from = s["from"];
        G[from].push_back(s);
    }
}

static json Distance;
static void ReadDistanceInformation() {  // 一个标准的邻接表表示的有向图
    ifstream ifstream_DistanceInformation("DistanceInformation.json");
    ifstream_DistanceInformation >> Distance;
}

static void ReadData() {
    ReadDatabase();
    CreateDGraph();
    ReadDistanceInformation();
}