#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <vector>
#include "MyAlgorithmScaffold.h"
#include "ReadData.cpp"

using namespace std;
using json = nlohmann::json;

struct cmp_plan_decrease {  // 优先队列的比较函数，总价小的被pop 掉的优先级高
    bool operator()(json a, json b) {
        return a["total_price"] < b["total_price"];  // 大的优先级高
    }
};

struct cmp_plan_increase {  // 优先队列的比较函数，总价大的被pop 掉的优先级高
    bool operator()(json a, json b) {
        return a["total_price"] > b["total_price"];  // 小的优先级高
    }
};

struct cmp_schedule_decrease {  // 优先队列的比较函数，总价大的被pop
                                // 掉的优先级高
    bool operator()(json a, json b) {
        return a["price"] < b["price"];  // 大的优先级高
    }
};

struct cmp_schedule_increase {  // 优先队列的比较函数，总价小的被pop
                                // 掉的优先级高
    bool operator()(json a, json b) {
        return a["price"] > b["price"];  // 小的优先级高
    }
};

struct cmp_schedule_Astar_f_increase {
    bool operator()(json a, json b) { return a["Astar_f"] > b["Astar_f"]; }
};

void dfs(priority_queue<json, vector<json>, cmp_plan_decrease> &ps, json &p,
         vector<string> &have_been_to, json schedule, string start,
         string end) {
    if (start == end) {
        // 如果抵达了
        p["path"] = chronologize(p["path"]);
        if (ps.size() < 1)
            ps.push(p);
        else {
            ps.pop();
            ps.push(p);
        }
    } else {
        priority_queue<json, vector<json>, cmp_schedule_increase> temp;

        for (json s : D) {  //  这里浪费太多时间了
            if (s["from"] == start) {
                temp.push(s);
            }
        }

        while (!temp.empty()) {
            json s = temp.top();
            temp.pop();

            if (!count(have_been_to.begin(), have_been_to.end(), s["to"])) {
                have_been_to.push_back(s["to"]);
                p["path"].push_back(s);
                p["total_price"] = int(p["total_price"]) + int(s["price"]);
                // 为什么这里需要类型转换？
                if (!ps.size()) {
                    dfs(ps, p, have_been_to, s, s["to"], end);
                }  // 剪枝
                else if (p["total_price"] < ps.top()["total_price"]) {
                    dfs(ps, p, have_been_to, s, s["to"], end);
                }
                p["total_price"] = int(p["total_price"]) - int(s["price"]);
                p["path"].erase(p["path"].end() - 1);
                have_been_to.pop_back();
            }
        }
    }
}

priority_queue<json, vector<json>, cmp_plan_decrease> DFS(
    string from, string to, vector<int> time_limit) {
    priority_queue<json, vector<json>, cmp_plan_decrease> ps;
    json p;
    p["path"] = {};
    p["total_price"] = 0;
    p["arrived_time"] = 0;
    vector<string> have_been_to{"Beijing"};

    dfs(ps, p, have_been_to, NULL, from, to, time_limit);
    return ps;
}

// 使用说明
int main() {
    ReadDatabase();
    cout << "系统加载完成。" << endl;
    priority_queue<json, vector<json>, cmp_plan_decrease> ps =
        DFS("北京", "上海", {10, 0, 0});
    while (!ps.empty()) {
        json p = ps.top();
        ps.pop();
        cout << p["total_price"] << endl;
        for (json s : p["path"]) cout << s << endl;
        cout << endl;
    }

    return 0;
}
