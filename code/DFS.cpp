#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <vector>
#include "MyAlgorithmScaffold.h"
#include "ReadData.h"

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

struct cmp_schedule_increase {  // 优先队列的比较函数，总价小的被pop
                                // 掉的优先级高
    bool operator()(json a, json b) {
        return a["price"] > b["price"];  // 小的优先级高
    }
};

struct cmp_schedule_decrease {  // 优先队列的比较函数，总价大的被pop
                                // 掉的优先级高
    bool operator()(json a, json b) {
        return a["price"] < b["price"];  // 大的优先级高
    }
};

void dfs(priority_queue<json, vector<json>, cmp_plan_decrease> &ps, json &p,
         vector<string> &have_been_to, json schedule, string start,
         string end) {
    if (start == end) {
        p["path"] = chronologize(p["path"]);
        if (ps.size() < 3)
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

priority_queue<json, vector<json>, cmp_plan_decrease> DFS(string start,
                                                          string end) {
    priority_queue<json, vector<json>, cmp_plan_decrease> ps;
    json p;
    p["path"] = {};
    p["total_price"] = 0;
    vector<string> have_been_to{"Beijing"};
    dfs(ps, p, have_been_to, NULL, start, end);
    return ps;
}

// // 使用说明
// int main() {
//     ReadDatabase();
//     cout << "系统加载完成。" << endl;
//     priority_queue<json, vector<json>, cmp_plan_decrease> ps =
//         DFS("Beijing", "Shenzhen");
//     while (!ps.empty()) {
//         json p = ps.top();
//         ps.pop();
//         cout << p["total_price"] << endl;
//         for (json s : p["path"]) cout << s << endl;
//         cout << endl;
//     }

//     // 剪枝后北京到深圳可以很容易求出来了，而北京到东京依然是组合爆炸。
//     return 0;
// }

// // 413
// //
// {"data":[0,11,40],"duration":[0,5,47],"end":[0,17,27],"from":"Beijing","number":"Z35","price":93,"start":[0,11,40],"to":"Zhengzhou","type":"HSR"}
// //
// {"data":[1,9,40],"duration":[0,15,0],"end":[1,0,40],"from":"Zhengzhou","number":"7719","price":320,"start":[0,9,40],"to":"Shenzhen","type":"N"}

// // 413
// //
// {"data":[0,11,40],"duration":[0,5,47],"end":[0,17,27],"from":"Beijing","number":"Z35","price":93,"start":[0,11,40],"to":"Zhengzhou","type":"HSR"}
// //
// {"data":[1,11,0],"duration":[0,15,0],"end":[1,2,0],"from":"Zhengzhou","number":"7715","price":320,"start":[0,11,0],"to":"Shenzhen","type":"N"}

// // 413
// //
// {"data":[0,13,0],"duration":[0,5,41],"end":[0,18,41],"from":"Beijing","number":"Z97","price":93,"start":[0,13,0],"to":"Zhengzhou","type":"HSR"}
// //
// {"data":[1,9,40],"duration":[0,15,0],"end":[1,0,40],"from":"Zhengzhou","number":"7719","price":320,"start":[0,9,40],"to":"Shenzhen","type":"N"}
