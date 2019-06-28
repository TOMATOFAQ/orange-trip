#include "json.hpp"
#include <fstream>
#include <iostream>
#include <queue>
#include <set>
#include <vector>
using namespace std;
using json = nlohmann::json;

static json ReadDatabase()
{
    vector<json> D;
    ifstream ifstream_database("database.json");
    json temp;
    ifstream_database >> temp;

    for (json j : temp) {
        D.push_back(j);
    }
    return D;
}

static json CreateDGraph(json& D)
{ // 一个标准的邻接表表示的有向图
    json G = {};
    for (json s : D) {
        string from = s["from"];
        G[from].push_back(s);
    }
    return G;
}

static json ReadDistanceInformation()
{ // 一个标准的邻接表表示的有向图
    json Distance;
    ifstream ifstream_DistanceInformation("DistanceInformation.json");
    ifstream_DistanceInformation >> Distance;
    return Distance;
}
