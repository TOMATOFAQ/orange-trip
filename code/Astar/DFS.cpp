#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <vector>
#include "MyAlgorithmScaffold.h"
#include "ReadData.cpp"
#define FARE 0.5  // 862yuan/1846km
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

void dfs(priority_queue<json, vector<json>, cmp_plan_decrease> &ps, json &p,
         vector<string> &have_been_to, json schedule, string start, string end,
         vector<int> time_limit) {
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
                    dfs(ps, p, have_been_to, s, s["to"], end, time_limit);
                }  // 剪枝
                else if (p["total_price"] < ps.top()["total_price"]) {
                    dfs(ps, p, have_been_to, s, s["to"], end, time_limit);
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
/////////////////////////////////////////
struct cmp_schedule_Astar_f_increase {
    bool operator()(json a, json b) { return a["Astar_f"] > b["Astar_f"]; }
};

int Astar_f(int g, string from, string to) {
    int distance = int(Distance[from][to]);
    int h = distance * FARE;
    return g + h;
}

void formatted_output(json plan) {
    cout << "Plan Message:" << endl;
    cout << "Arrived time:" << plan["arrived_time"] << endl;
    cout << "Total price:" << plan["total_price"] << endl;
    cout << "Schedules:" << endl;
    for (json p : plan["path"]) {
        cout << p << endl;
    }
    cout << endl;
}

void adfs(string from, string to, vector<int> time_limit, int money_limit,
          set<string> have_been_to, json &plan, json &final_plan) {
    if (from == to) {
        plan["path"] = chronologize(plan["path"]);
        final_plan = plan;
        formatted_output(final_plan);
    } else {
        formatted_output(plan);

        priority_queue<json, vector<json>, cmp_schedule_Astar_f_increase>
            schedule_pq;
        for (json schedule : G[from]) {
            int schedule_price = int(schedule["price"]);
            schedule["Astar_f"] = Astar_f(schedule_price, from, to);
            schedule_pq.push(schedule);
        }

        while (!schedule_pq.empty()) {
            json schedule = schedule_pq.top();
            schedule_pq.pop();

            if (!have_been_to.count(schedule["to"])) {
                string schedule_to = schedule["to"];
                have_been_to.insert(schedule_to);
                plan["path"].push_back(schedule);
                plan["total_price"] =
                    int(plan["total_price"]) + int(schedule["price"]);
                /**/
                if (final_plan.empty()) {
                    adfs(schedule["to"], to, time_limit, money_limit,
                         have_been_to, plan, final_plan);
                } else if (plan["totoal_price"] < final_plan["totoal_price"]) {
                    adfs(schedule["to"], to, time_limit, money_limit,
                         have_been_to, plan, final_plan);
                }
                cout << plan["total_price"] << endl;
                ;
                plan["total_price"] =
                    int(plan["total_price"]) - int(schedule["price"]);
                plan["path"].erase(plan["path"].end() - 1);
                have_been_to.erase(schedule_to);
            }
        }
    }
}

json ADFS(string from, string to, vector<int> time_limit, int money_limit) {
    json plan;
    plan["path"] = {};
    plan["total_price"] = 0;
    plan["arrived_time"] = 0;
    json final_plan;
    set<string> have_been_to = {from};
    adfs(from, to, time_limit, money_limit, have_been_to, plan, final_plan);
    return final_plan;
}

int main() {
    ReadData();
    cout << "系统加载完成。" << endl;
    json final_plan = ADFS("北京", "成都", {10, 0, 0}, 100000);
    formatted_output(final_plan);
    return 0;
}