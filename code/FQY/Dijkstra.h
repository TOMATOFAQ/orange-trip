#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
#include <vector>
#include "ReadData.h"
using namespace std;
using json = nlohmann::json;

// 给当前时间 t1加上一段绝对时间t2
vector<int> addtime_absolutely(vector<int> t1, vector<int> t2) {
    t1[2] = t1[2] + t2[2];
    if (t1[2] >= 60) {
        t1[2] = t1[2] - 60;
        t1[1] += 1;
    }
    t1[1] += t2[1];
    if (t1[1] >= 24) {
        t1[1] = t1[1] - 24;
        t1[0] += 1;
    }
    t1[0] += t2[0];
    return t1;
}

// 结合当前时间，返回抵达时间
vector<int> addtime_by_schedule(vector<int> t, json s) {
    vector<int> ss = s["start"];
    vector<int> se = s["end"];
    if (t[1] > ss[1] || (t[1] == ss[1] && t[2] > ss[2])) {
        t[0]++;
    }
    t[0] += se[0];
    t[1] = se[1];
    t[2] = se[2];

    return t;
}

// 优先队列的比较函数，值小的被pop掉的优先级高
struct cmp_schedule_time_increase {
    bool operator()(json a, json b) {
        return a["arrive_time"] > b["arrive_time"];
    }
};

// 优先队列的比较函数，值小的被pop掉的优先级高
struct cmp_schedule_money_increase {
    bool operator()(json a, json b) {
        return a["arrive_money"] > b["arrive_money"];
    }
};

// 给顺序的 schedule list 配上时间
vector<int> addtime_by_schedule_by_start_time(vector<int> t, json s) {
    vector<int> ss = s["start"];
    vector<int> se = s["end"];
    if (t[1] > ss[1] || (t[1] == ss[1] && t[2] > ss[2])) {
        t[0]++;
    }
    t[0] += ss[0];
    t[1] = ss[1];
    t[2] = ss[2];
    return t;
}

json chronologize(json sl) {
    vector<int> ct = {0, 0, 0};  // current_time
    for (json &s : sl) {
        ct = addtime_by_schedule_by_start_time(ct, s);
        s["data"] = ct;
    }
    return sl;
}

//返回目标节点的status，包括花费金钱，花费时间以及 schedule 序列
template <typename T>
json dijkstra(string from, string to, vector<int> t) {
    // 初始化
    set<string> vis = {from};
    json least_cost_schedule = {
        {"start", {0, 0, 0}}, {"end", {MAX, MAX, MAX}}, {"price", MAX}};
    json status;
    for (auto it = G.begin(); it != G.end(); it++) {
        status[it.key()]["t"] = {MAX, MAX, MAX};
        status[it.key()]["m"] = MAX;
        status[it.key()]["s"].push_back(least_cost_schedule);
    }
    status[from]["t"] = t;
    status[from]["m"] = 0;
    status[from]["s"] = {};

    // dijkstra 过程
    priority_queue<json, vector<json>, T> pq;
    string current_city = from;
    while (!vis.count(to)) {
        // 更新新加节点后所的各节点状态
        for (json s : G[current_city]) {
            string to = s["to"];
            string from = s["from"];
            vector<int> time =
                addtime_by_schedule(status[current_city]["t"], s);
            int money = int(status[from]["m"]) + int(s["price"]);
            if ((typeid(T) == typeid(struct cmp_schedule_money_increase) &&
                 money < status[to]["m"]) ||
                (typeid(T) == typeid(struct cmp_schedule_time_increase) &&
                 time < status[to]["t"])) {
                status[to]["t"] = time;
                status[to]["m"] = money;
                status[to]["s"].erase(--status[to]["s"].end());
                string from = s["from"];
                status[to]["s"] = status[from]["s"];
                status[to]["s"].push_back(s);
                json pqj = {{"s", s},
                            {"arrive_time", time},
                            {"arrive_money", status[to]["m"]}};
                pq.push(pqj);
            }
        }

        // 选取目前集合中权值最小的路径，将其终点加入到to集合中
        do {
            json pqj = pq.top();
            least_cost_schedule = pqj["s"];
            pq.pop();
        } while (vis.count(least_cost_schedule["to"]));

        current_city = least_cost_schedule["to"];
        vis.insert(current_city);
        // cout << current_city << endl; // 用于观察节点的加入顺序
    }

    // 只对最后求得的目标节点的行程安排安插时序
    status[to]["s"] = chronologize(status[to]["s"]);

    return status[to];
}

// int main() {
//     ReadDatabase();
//     CreateDGraph();
//     cout << "加载完毕。" << endl;

//     vector<int> t = {0, 15, 44};
//     json to_status;

//     to_status =
//         dijkstra<struct cmp_schedule_money_increase>("Beijing", "Shenzhen",
//         t);
//     for (json p : to_status) {
//         cout << p << endl;
//     }

//     to_status =
//         dijkstra<struct cmp_schedule_time_increase>("Beijing", "Shenzhen",
//         t);
//     for (json p : to_status) {
//         cout << p << endl;
//     }

//     return 0;
// }