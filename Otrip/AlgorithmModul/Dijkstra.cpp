#include "json.hpp"
#include <iostream>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#define MAX 99999
#define PRIORITY int
#define MONEY 1
#define TIME 2
using namespace std;
using json = nlohmann::json;

class Dijkstra {
public:
    /* 类初始化函数 */
    Dijkstra() {}
    Dijkstra(json& G)
        : G(G)
    {
    }

    /* 接口函数 */
    json dijkstra(string from,
        string to,
        PRIORITY priority,
        vector<int> startTime)
    {
        if (priority == TIME) {
            return dijkstraProcess<struct cmp_schedule_time_increase>(
                from, to, startTime);
        } else {
            return dijkstraProcess<struct cmp_schedule_money_increase>(
                from, to, startTime);
        }
    }

    json dijkstraSearchPathAmongCities(string from,
        string to,
        vector<string> via,
        vector<int> startTime = { 0, 0, 0 },
        PRIORITY priority = MONEY)
    {
        json plan = { { "totalPrice", MAX }, { "arrivedTime", { MAX, MAX, MAX } } };
        json planOfXXXXPriority = { { "totalPrice", MAX }, { "arrivedTime", { MAX, MAX, MAX } } };
        json planOfTimePriority = { { "totalPrice", MAX }, { "arrivedTime", { MAX, MAX, MAX } } };
        json tempPlanOfXXXXPriority;
        json tempPlanOfTimePriority;
        map<string, bool> vis;

        // 取得金钱最佳
        // 需要检测是否已经在经历城市之中
        do {
            tempPlanOfXXXXPriority = { { "totalPrice", 0 }, { "arrivedTime", { 0, 0, 0 } } };
            // 对于一个特定的 permutation，两两城市依次搜索
            vector<string> currentVia = via; // 这里为什么要声明currentVia???直接用 via 就可以了啊
            for (string city : via) {
                vis[city] = false;
            }

            string currentFrom = from;
            json currentTime = startTime;

            // 从from 开始，遍历该permutation下的城市序列
            for (string currentTo : currentVia) {
                if (!vis[currentTo]) {
                    tempPlanOfXXXXPriority = PlanPlusPlan(tempPlanOfXXXXPriority, dijkstra(currentFrom, currentTo, priority, currentTime), startTime);
                    currentFrom = currentTo;
                    currentTime = tempPlanOfXXXXPriority["arrivedTime"];
                    for (string city : getVis(tempPlanOfXXXXPriority)) {
                        vis[city] = true;
                    }
                }
            }

            // 从倒数第二个城市到to的路径
            tempPlanOfXXXXPriority = PlanPlusPlan(tempPlanOfXXXXPriority, dijkstra(currentFrom, to, priority, currentTime), startTime);

            if (tempPlanOfXXXXPriority["totalPrice"] < planOfXXXXPriority["totalPrice"])
                planOfXXXXPriority = tempPlanOfXXXXPriority;

            //            cout << tempPlanOfXXXXPriority["totalPrice"] << endl;

        } while (next_permutation(via.begin(), via.end()));

        return planOfXXXXPriority;
    }

private:
    /* 类内全局变量 */
    json G;

    /* 类内函数 */

    /* 1️⃣ 工具函数 */

    struct cmp_schedule_time_increase {
        bool operator()(json a, json b)
        {
            return a["arrive_time"] > b["arrive_time"];
        }
    };

    // 优先队列的比较函数，值小的被pop掉的优先级高
    struct cmp_schedule_money_increase {
        bool operator()(json a, json b)
        {
            return a["arrive_money"] > b["arrive_money"];
        }
    };

    // 结合当前时间，返回抵达时间
    static vector<int> addtime_by_schedule(vector<int> t, json s)
    {
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

    static json chronologize(json scheduleList, vector<int> startTime)
    {
        vector<int> ct = startTime;
        for (json& s : scheduleList) {
            vector<int> ss = s["start"];
            vector<int> se = s["end"];
            if (ct[1] > ss[1] || (ct[1] == ss[1] && ct[2] > ss[2])) {
                ct[0]++;
            }
            ct[0] += ss[0];
            ct[1] = ss[1];
            ct[2] = ss[2];

            s["date"] = ct; // 乘车前
            ct = addtime_by_schedule(ct, s); // 乘车后
            s["arrivedTime"] = ct;
        }
        return scheduleList;
    }

    json PlanPlusPlan(json p1, json p2, vector<int> startTime)
    {
        json plan;
        plan["totalPrice"] = p1["totalPrice"].get<int>() + p2["totalPrice"].get<int>();
        json arrivedTime = p1["arrivedTime"];
        // cout << p1["arrivedTime"] << endl;
        // 这里找到一个bug,为什么p1["arrivedTime"]里的值会是 [0,0,0] 呢？不管了，没心情找 bug 了直接在后面修正吧。
        json scheduleList = p1["scheduleList"];
        for (const json& s : p2["scheduleList"]) {
            scheduleList.push_back(s);
            arrivedTime = addtime_by_schedule(arrivedTime, const_cast<json&>(s));
            // cout << arrivedTime <<endl;
        }
        // plan["arrivedTime"] = arrivedTime;
        scheduleList = chronologize(scheduleList, startTime);
        plan["scheduleList"] = scheduleList;
        plan["arrivedTime"] = scheduleList.back()["arrivedTime"];
        return plan;
    }

    set<string> getVis(json& plan)
    {
        set<string> vis{ plan["scheduleList"][0]["from"].get<string>() };
        for (json s : plan["scheduleList"]) {
            vis.insert(s["from"].get<string>());
        }
        return vis;
    }

    /* 2️⃣主要功能函数 */

    //返回目标节点的status，包括花费金钱，花费时间以及 schedule 序列
    template <typename T>
    json dijkstraProcess(string& from, string& to, vector<int> startTime)
    {
        // 0️⃣初始化
        set<string> vis = { from };

        json least_cost_schedule = { // 一个最小花费的模板，默认是最大值
            { "start", { 0, 0, 0 } },
            { "end", { MAX, MAX, MAX } },
            { "price", MAX }
        };

        json status; // 图中各个节点的状态
        for (auto it = G.begin(); it != G.end(); it++) {
            status[it.key()]["arrivedTime"] = { MAX, MAX, MAX };
            status[it.key()]["totalPrice"] = MAX;
            status[it.key()]["scheduleList"].push_back(least_cost_schedule);
        }

        // 初始化起始点的状态
        status[from]["arrivedTime"] = startTime;
        status[from]["totalPrice"] = 0;
        status[from]["scheduleList"] = {};

        // dijkstra 过程
        priority_queue<json, vector<json>, T> pq; // 一个用来存储所有路径的优先队列
        string current_city = from;

        while (!vis.count(to)) { // 当终点不在dijkstra的访问过的城市中

            // 1️⃣更新新加节点后所的各节点状态
            for (json schedule : G[current_city]) {
                string current_to = schedule["to"];

                vector<int> time = addtime_by_schedule(
                    status[current_city]["arrivedTime"], schedule);
                int money = int(status[current_city]["totalPrice"]) + int(schedule["price"]);

                if ((typeid(T) == typeid(struct cmp_schedule_money_increase) && money < status[current_to]["totalPrice"]) || (typeid(T) == typeid(struct cmp_schedule_time_increase) && time < status[current_to]["arrivedTime"])) {
                    status[current_to]["arrivedTime"] = time;
                    status[current_to]["totalPrice"] = money;
                    status[current_to]["scheduleList"] = status[current_city]["scheduleList"];
                    status[current_to]["scheduleList"].push_back(schedule);

                    json pqj = {
                        { "scheduleList", schedule },
                        { "arrive_time", time },
                        { "arrive_money", status[current_to]["totalPrice"] }
                    };

                    pq.push(pqj);
                }
            }

            // 2️⃣选取目前集合中权值最小的路径，将其终点加入到to集合中
            do {
                json pqj = pq.top();
                least_cost_schedule = pqj["scheduleList"];
                pq.pop();
            } while (vis.count(least_cost_schedule["to"])); // 一直弹出直到终点是新的城市

            current_city = least_cost_schedule["to"];
            vis.insert(current_city);
        }

        // 只对最后求得的目标节点的行程安排安插时序，以节省算力
        status[to]["scheduleList"] = chronologize(status[to]["scheduleList"], startTime);
        status[to]["arrivedTime"] = status[to]["scheduleList"].back()["arrivedTime"];

        // status 的数据结构恰好与 plan 相同，所以最后返回 plan
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
