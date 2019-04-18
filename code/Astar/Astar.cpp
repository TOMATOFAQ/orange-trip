#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
#include <vector>
#include "ReadData.cpp"
#define FARE 0.5  // 862yuan/1846km
using namespace std;
using json = nlohmann::json;

struct cmp_schedule_Astar_f_increase {
    bool operator()(json a, json b) { return a["Astar_f"] > b["Astar_f"]; }
};

int Astar_f(int g, string from, string to) {
    int distance = int(Distance[from][to]);
    int h = distance * FARE;
    return g + h;
}

json dfs(string from, string to, vector<int> start_time,
         vector<int> time_limit) {}

json Astar(string from, string to, vector<int> start_time,
           vector<int> time_limit) {
    json plan;
    priority_queue<json, vector<json>, cmp_schedule_Astar_f_increase> pq;
    set<json> open_list;
    set<json> closed_list;

    
    return plan;
}

int main() {
    ReadData();
    cout << "加载完毕。" << endl;

    vector<int> start_time = {0, 0, 0};
    vector<int> time_limit = {0, 0, 0};
    json to_status;
    Astar("北京", "上海", start_time, time_limit);
    cout << Astar_f(0, "广州", "北京");

    return 0;
}