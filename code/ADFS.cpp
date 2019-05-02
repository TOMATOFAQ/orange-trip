#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
#include <sstream>
#include <vector>
#define MAX 99999
#define FARE 0.5    // 862元/1846km
#define RELAXION 1  // 松弛程度，值为 1 时表示不松弛
using namespace std;
using json = nlohmann::json;

/* 尽管剪枝已经减了很多，搜索时间（30s，北京到广州）花得时间还是太长了 */

class AstarDFS {
   public:
    /* 类初始化函数 */
    AstarDFS(){};
    AstarDFS(json& G, json& Distance)
        : G(G), Distance(Distance) {}

    /* 接口函数 */
    json astarDFS(string& from,
                  string& to,
                  vector<int>& startTime,
                  vector<int>& time_limit,
                  int money_limit) {
        return ADFS(from,
                    to,
                    startTime,
                    time_limit,
                    money_limit);
    }

   private:
    /*类内全局变量*/
    json G;
    json Distance;
    /*类内函数*/

    /* 1️⃣ 工具函数 */
    struct cmp_schedule_Astar_f_increase {
        bool operator()(json a, json b) {
            if (a["Astar_f"] == b["Astar_f"]) {
                return a["end"] > b["end"];
            } else
                return a["Astar_f"] > b["Astar_f"];
        }
    };

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

    json chronologize(json sl) {
        vector<int> ct = {0, 0, 0};  // current_time
        for (json& s : sl) {
            vector<int> ss = s["start"];
            vector<int> se = s["end"];
            if (ct[1] > ss[1] || (ct[1] == ss[1] && ct[2] > ss[2])) {
                ct[0]++;
            }
            ct[0] += ss[0];
            ct[1] = ss[1];
            ct[2] = ss[2];

            s["date"] = ct;
        }
        return sl;
    }

    int Astar_f(int g, string from, string to) {  // Astar 中的启发函数f(n) = g(n) + h(n)
        int distance = int(Distance[from][to]);
        int h = distance * FARE;
        return g + h;
    }

    /* 2️⃣主要功能函数 */

    // 初始化函数
    json ADFS(string& from,
              string& to,
              vector<int>& startTime,
              vector<int>& time_limit,
              int& money_limit) {
        // 初始化
        json plan;
        plan["scheduleList"] = {};
        plan["totalPrice"] = 0;
        plan["arrivedTime"] = {0, 0, 0};
        json final_plan;
        set<string> vis = {from};
        // 图中各个节点的状态
        json graphStatus;
        for (auto it = G.begin(); it != G.end(); it++) {
            graphStatus[it.key()]["arrivedTime"] = {MAX, MAX, MAX};
            graphStatus[it.key()]["totalPrice"] = MAX;
            graphStatus[it.key()]["ScheduleList"] = {};  // Price and Time
        }
        graphStatus[from]["arrivedTime"] = startTime;
        graphStatus[from]["totalPrice"] = 0;

        //进行 A-star 优化过的深搜
        adfs(from, to, time_limit, money_limit, vis, graphStatus, plan, final_plan);
        return final_plan;
    }

    void adfs(const string& currentFrom,
              const string& finallyTo,
              const vector<int>& time_limit,
              const int& money_limit,
              set<string>& vis,
              json& graphStatus,
              json& plan,
              json& final_plan) {
        if (currentFrom == finallyTo) {  // 如果抵达终点
            if (final_plan["totalPrice"].is_null() /*第一个抵达终点的计划*/ ||
                plan["totalPrice"] < final_plan["totalPrice"] /*总价比先前计划要低的计划*/) {
                plan["scheduleList"] = chronologize(plan["scheduleList"]);
                final_plan = plan;
            }
        } else {
            // 建立优先队列并计算每一个行程的启发函数值
            priority_queue<json, vector<json>, cmp_schedule_Astar_f_increase> schedule_pq;
            for (json schedule : G[currentFrom]) {
                int schedule_price = int(schedule["price"]);
                string schedule_to = schedule["to"];
                schedule["Astar_f"] = Astar_f(schedule_price, schedule_to, finallyTo);
                schedule_pq.push(schedule);
            }

            int currentFromAstar_f = Astar_f(plan["totalPrice"], currentFrom, finallyTo);
            currentFromAstar_f *= RELAXION;  // 松弛操作，留有余地。后文解释为什么

            // 遍历每一个行程
            while (!schedule_pq.empty()) {
                json schedule = schedule_pq.top();
                schedule_pq.pop();

                if (!vis.count(schedule["to"])) {  // 如果该行程的目的地在当前 plan 中还未经过，则尝试

                    string schedule_to = schedule["to"];  // 之所以这样倒腾来倒腾去是为类型转换
                    string schedule_from = schedule["from"];

                    vis.insert(schedule_to);                   // 加入到已抵达城市集合
                    plan["scheduleList"].push_back(schedule);  // 将该行程加入到plan中
                    plan["totalPrice"] = int(plan["totalPrice"]) + int(schedule["price"]);
                    vector<int> arrivedTime = addtime_by_schedule(plan["arrivedTime"], schedule);
                    vector<int> previousArrivedTime = plan["arrivedTime"];
                    plan["arrivedTime"] = arrivedTime;

                    // 语言技术上的问题：因为键名需要是字符串，故这里需要用某种方式将整形转换为字符串
                    stringstream sstr;
                    int temp = schedule["price"];
                    temp /= 100;
                    sstr << temp;
                    string tempPriceStr = sstr.str();

                    // 第一次剪枝：剪同样价格但不同时间的航班
                    int ifBetter = 0;
                    // 更新同价格但抵达时间不同的航班的最早抵达时间
                    if (graphStatus[schedule_to]["ScheduleList"][tempPriceStr].is_null() || plan["arrivedTime"] < graphStatus[schedule_to]["ScheduleList"][tempPriceStr]) {
                        graphStatus[schedule_to]["ScheduleList"][tempPriceStr] = plan["arrivedTime"];
                    }
                    if (plan["arrivedTime"] <= graphStatus[schedule_to]["ScheduleList"][tempPriceStr] /*对同一种航班剪枝*/
                        && arrivedTime <= time_limit && plan["totalPrice"] <= money_limit) {
                        ++ifBetter;
                    }

                    // 第二次剪枝：剪远离目的地的行程
                    if (currentFromAstar_f > schedule["Astar_f"]) {  // 启发函数下降，说明离搜索结果更近了。而之前的松弛操作是为了允许远离目的地一点点以保证搜索出最优解
                        ++ifBetter;
                    }

                    // 第 n 次剪枝：其余剪枝技术

                    if (ifBetter == 2) {
                        // 最后一次剪枝:如果还没有一个抵达目的地的可行计划，则不剪枝（因为没有参考对象）。如果有，则将比较当前花费与一个可行计划比较。在一个计划在没有抵达目的地的情况下花费已经比一个可行计划多时，舍之
                        // /*观察*/ cout << schedule["from"] << schedule["to"] << schedule["Astar_f"] << " " << plan["totalPrice"] << " " << json(arrivedTime) << " " << money_limit << " " << json(time_limit) << final_plan["totalPrice"] << endl;
                        if (final_plan["totalPrice"].is_null()) {
                            adfs(schedule_to, finallyTo, time_limit, money_limit, vis, graphStatus, plan, final_plan);
                        } else if (plan["totalPrice"] <= final_plan["totalPrice"]) {
                            adfs(schedule_to, finallyTo, time_limit, money_limit, vis, graphStatus, plan, final_plan);
                        }
                    }

                    // 已经尝试过这条路径，将尝试所留下的痕迹抹除，所有信息恢复到尝试该路径之前的状态。准备尝试下一个行程
                    plan["arrivedTime"] = previousArrivedTime;
                    plan["totalPrice"] = int(plan["totalPrice"]) - int(schedule["price"]);
                    plan["scheduleList"].erase(plan["scheduleList"].end() - 1);
                    vis.erase(schedule_to);
                }
            }
        }
    }
};
