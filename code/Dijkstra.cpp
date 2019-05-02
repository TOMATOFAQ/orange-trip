#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#define MAX 99999
using namespace std;
using json = nlohmann::json;

class Dijkstra {
   public:
    /* 类初始化函数 */

    Dijkstra() {}
    Dijkstra(json& G)
        : G(G) {}

    /* 接口函数 */

    json dijkstra(string from,
                  string to,
                  string priority,
                  vector<int> startTime) {
        if (priority == "time") {
            return dijkstraProcess<struct cmp_schedule_time_increase>(
                from, to, startTime);
        } else {
            return dijkstraProcess<struct cmp_schedule_money_increase>(
                from, to, startTime);
        }
    }

   private:
    /* 类内全局变量 */
    json G;

    /* 类内函数 */

    /* 1️⃣ 工具函数 */

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

    // 给顺序的 schedule list 配上时间

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

    /* 2️⃣主要功能函数 */

    //返回目标节点的status，包括花费金钱，花费时间以及 schedule 序列
    template <typename T>
    json dijkstraProcess(string from, string to, vector<int> t) {
        // 0️⃣初始化
        set<string> vis = {from};

        json least_cost_schedule = {// 一个最小花费的模板，默认是最大值
                                    {"start", {0, 0, 0}},
                                    {"end", {MAX, MAX, MAX}},
                                    {"price", MAX}};

        json status;  // 图中各个节点的状态
        for (auto it = G.begin(); it != G.end(); it++) {
            status[it.key()]["arrivedTime"] = {MAX, MAX, MAX};
            status[it.key()]["totalPrice"] = MAX;
            status[it.key()]["scheduleList"].push_back(least_cost_schedule);
        }

        // 初始化起始点的状态
        status[from]["arrivedTime"] = t;
        status[from]["totalPrice"] = 0;
        status[from]["scheduleList"] = {};

        // dijkstra 过程
        priority_queue<json, vector<json>, T>
            pq;  // 一个用来存储所有路径的优先队列
        string current_city = from;

        while (!vis.count(to)) {  // 当终点不在dijkstra的访问过的城市中

            // 1️⃣更新新加节点后所的各节点状态
            for (json schedule : G[current_city]) {
                string current_to = schedule["to"];

                vector<int> time = addtime_by_schedule(
                    status[current_city]["arrivedTime"], schedule);
                int money = int(status[current_city]["totalPrice"]) +
                            int(schedule["price"]);

                if ((typeid(T) == typeid(struct cmp_schedule_money_increase) &&
                     money < status[current_to]["totalPrice"]) ||
                    (typeid(T) == typeid(struct cmp_schedule_time_increase) &&
                     time < status[current_to]["arrivedTime"])) {
                    status[current_to]["arrivedTime"] = time;
                    status[current_to]["totalPrice"] = money;
                    status[current_to]["scheduleList"] =
                        status[current_city]["scheduleList"];
                    status[current_to]["scheduleList"].push_back(schedule);

                    json pqj = {
                        {"scheduleList", schedule},
                        {"arrive_time", time},
                        {"arrive_money", status[current_to]["totalPrice"]}};

                    pq.push(pqj);
                }
            }

            // 2️⃣选取目前集合中权值最小的路径，将其终点加入到to集合中
            do {
                json pqj = pq.top();
                least_cost_schedule = pqj["scheduleList"];
                pq.pop();
            } while (vis.count(
                least_cost_schedule["to"]));  // 一直弹出直到终点是新的城市

            current_city = least_cost_schedule["to"];
            vis.insert(current_city);
            // cout << current_city << endl;  // 用于观察节点的加入顺序
        }

        // 只对最后求得的目标节点的行程安排安插时序.
        status[to]["scheduleList"] = chronologize(status[to]["scheduleList"]);

        return status[to];
    }
};
// int main() {
//     cout << "加载完毕。" << endl;

//     vector<int> startTime = {0, 0, 0};
//     json to_status;

//     json D = ReadDatabase();
//     json G = CreateDGraph(D);
//     Dijkstra dijkstra(G);

//     to_status = dijkstra.dijkstra("北京", "广州", "time", startTime);

//     to_status = dijkstra.dijkstra("北京", "广州", "money", startTime);

//     return 0;
// }
