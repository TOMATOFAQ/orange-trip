#include "json.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <vector>
#define MAX 99999
#define FARE 0.5 // 862元/1846km
#define RELAXION 1 // 松弛程度，值为 1 时表示不松弛 // 松弛程度为 2 表示所有路径都折返一遍。一个比较合适的松弛程度
#define RELAXION_AMONG_CITY 1 // 松弛程度，值为 1 时表示不松弛 // 松弛程度为 2 表示所有路径都折返一遍。一个比较合适的松弛程度
#define PRIORITY int
#define MONEY 1
#define TIME 2
using namespace std;
using json = nlohmann::json;

/* 尽管剪枝已经减了很多，搜索时间（debug模式下编译运行30s，北京到广州。但最起码已经具有可行性)花得时间还是太长了 */
/* 不过 release 模式下编译运行居然只需要15s，快了一倍啊！那完全是可以接受的了 */

class AstarDFS {
public:
    /* 类初始化函数 */
    AstarDFS(){};
    AstarDFS(json& G, json& Distance)
        : G(G)
        , Distance(Distance)
    {
    }

    /* 接口函数 */
    json astarDFS(string from,
        string to,
        vector<int> startTime,
        vector<int> timeLimit,
        int moneyLimit,
        PRIORITY priority)
    {
        return ADFS(from,
            to,
            startTime,
            timeLimit,
            moneyLimit,
            priority);
    }

    json aStarDFSAmongCities(string from,
        string to,
        vector<string> via,
        vector<int> startTime,
        vector<int> timeLimit,
        int moneyLimit,
        PRIORITY priority)
    {
        return ADFSAmongCities(from,
            to,
            via,
            startTime,
            timeLimit,
            moneyLimit,
            priority);
    }

private:
    /*类内全局变量*/
    json G;
    json Distance;
    /*类内函数*/

    /* 1️⃣ 工具函数 */
    struct cmp_schedule_Astar_f_increase {
        bool operator()(json a, json b)
        {
            if (a["Astar_f"] == b["Astar_f"]) {
                return a["end"] > b["end"];
            } else
                return a["Astar_f"] > b["Astar_f"];
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

    int Astar_f(int g, string from, string to)
    { // Astar 中的启发函数f(n) = g(n) + h(n)
        int distance = int(Distance[from][to]);
        int h = distance * FARE;
        return g + h;
    }

    /* 2️⃣主要功能函数 */

    // 初始化函数
    json ADFS(string from,
        string to,
        vector<int> startTime,
        vector<int> timeLimit,
        int moneyLimit,
        PRIORITY priority)
    {
        // 初始化
        json plan;
        plan["scheduleList"] = {};
        plan["totalPrice"] = 0;
        plan["arrivedTime"] = startTime;
        json final_plan;
        map<string, bool> vis;
        vis[from] = true;
        // 图中各个节点的状态
        json graphStatus;
        for (auto it = G.begin(); it != G.end(); it++) {
            graphStatus[it.key()]["arrivedTime"] = { MAX, MAX, MAX };
            graphStatus[it.key()]["totalPrice"] = MAX;
            graphStatus[it.key()]["scheduleList"] = {}; // Price and Time
        }
        graphStatus[from]["arrivedTime"] = startTime;
        graphStatus[from]["totalPrice"] = 0;

        //进行 A-star 优化过的深搜
        adfs(from, to, timeLimit, moneyLimit, priority, vis, graphStatus, plan, final_plan);
        final_plan["scheduleList"] = chronologize(final_plan["scheduleList"], startTime);
        return final_plan;
    }

    void adfs(const string& currentFrom,
        const string& finallyTo,
        const vector<int>& timeLimit,
        const int& moneyLimit,
        PRIORITY& priority,
        map<string, bool>& vis,
        json& graphStatus,
        json& plan,
        json& final_plan)
    {
        if (currentFrom == finallyTo) { // 如果抵达终点
            if (final_plan["totalPrice"].is_null() /*第一个抵达终点的计划*/
                || (priority == MONEY && plan["totalPrice"] < final_plan["totalPrice"]) /*总价比先前计划要低的计划*/
                || (priority == TIME && plan["arrivedTime"] < final_plan["arrivedTime"]) /*抵达时间比先前计划要早的计划*/) {
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
            currentFromAstar_f *= RELAXION; // 松弛操作，留有余地。后文解释为什么

            // 遍历每一个行程
            while (!schedule_pq.empty()) {
                json schedule = schedule_pq.top();
                schedule_pq.pop();

                if (!vis[schedule["to"]]) { // 如果该行程的目的地在当前 plan 中还未经过，则尝试

                    string schedule_to = schedule["to"]; // 之所以这样倒腾来倒腾去是为类型转换 // 之所以这样倒腾来倒腾去是因为当时不知道有get<string>()这个直接从 json 中读数据的函数。不过现在这样临时取出来应该也能加快速度，毕竟少了一次调度
                    string schedule_from = schedule["from"];

                    vis[schedule_to] = true; // 加入到已抵达城市集合
                    plan["scheduleList"].push_back(schedule); // 将该行程加入到plan中
                    plan["totalPrice"] = int(plan["totalPrice"]) + int(schedule["price"]);
                    vector<int> arrivedTime = addtime_by_schedule(plan["arrivedTime"], schedule);
                    vector<int> previousArrivedTime = plan["arrivedTime"];
                    plan["arrivedTime"] = arrivedTime;

                    // c++语言技术上的问题：因为json键名需要是字符串，故这里需要用某种方式将整形转换为字符串
                    // stringstream sstr;
                    // int temp = schedule["price"];
                    // temp /= 100;
                    // sstr << temp;
                    // string tempPriceStr = sstr.str();

                    // 我快感动哭了。c++库居然带了 int 转换成 string。上面那个建立输入输出流可浪费了好多性能啊...
                    //                      不对。为什么使用了自带的库函数，速度反而慢到令人发指呢
                    string tempPriceStr = to_string(schedule["price"].get<int>() / 100);

                    // 第一次剪枝：剪同样价格但不同时间的航班
                    int ifBetter = 0;
                    // 更新同价格但抵达时间不同的航班的最早抵达时间
                    if (graphStatus[schedule_to]["scheduleList"][tempPriceStr].is_null() || plan["arrivedTime"] < graphStatus[schedule_to]["scheduleList"][tempPriceStr]) {
                        graphStatus[schedule_to]["scheduleList"][tempPriceStr] = plan["arrivedTime"];
                    }
                    if (plan["arrivedTime"] <= graphStatus[schedule_to]["scheduleList"][tempPriceStr] /*对同一种航班剪枝*/
                        && arrivedTime <= timeLimit && plan["totalPrice"] <= moneyLimit) {
                        ++ifBetter;
                    }

                    // 第二次剪枝：剪远离目的地的行程
                    if (currentFromAstar_f > schedule["Astar_f"]) { // 启发函数下降，说明离搜索结果更近了。而之前的松弛操作是为了允许远离目的地一点点以保证搜索出最优解
                        ++ifBetter;
                    }

                    // 第 n 次剪枝：其余剪枝技术

                    if (ifBetter == 2) {
                        // 最后一次剪枝:如果还没有一个抵达目的地的可行计划，则不剪枝（因为没有参考对象）。如果有，则将比较当前花费与一个可行计划比较。在一个计划在没有抵达目的地的情况下花费已经比一个可行计划多时，舍之
                        if (final_plan["totalPrice"].is_null()) {
                            adfs(schedule_to, finallyTo, timeLimit, moneyLimit, priority, vis, graphStatus, plan, final_plan);
                        } else if ((priority == MONEY && plan["totalPrice"] < final_plan["totalPrice"]) /*总价比先前计划要低的计划*/
                            || (priority == TIME && plan["arrivedTime"] < final_plan["arrivedTime"]) /*抵达时间比先前计划要早的计划*/) {
                            adfs(schedule_to, finallyTo, timeLimit, moneyLimit, priority, vis, graphStatus, plan, final_plan);
                        }
                    }

                    // 已经尝试过这条路径，将尝试所留下的痕迹抹除，所有信息恢复到尝试该路径之前的状态。准备尝试下一个行程
                    plan["arrivedTime"] = previousArrivedTime;
                    plan["totalPrice"] = int(plan["totalPrice"]) - int(schedule["price"]);
                    plan["scheduleList"].erase(plan["scheduleList"].end() - 1);
                    vis[schedule_to] = false;
                }
            }
        }
    }

    /**************************多城市搜索-两个函数内容几乎一样，多一个 via 的判断尔*****************************/
    json ADFSAmongCities(string from,
        string to,
        const vector<string> via,
        vector<int> startTime,
        vector<int> timeLimit,
        int moneyLimit,
        PRIORITY priority)
    {
        // 初始化
        json plan;
        plan["scheduleList"] = {};
        plan["totalPrice"] = 0;
        plan["arrivedTime"] = startTime;
        json final_plan;
        map<string, bool> vis;
        vis[from] = true;
        // 图中各个节点的状态
        json graphStatus;
        for (auto it = G.begin(); it != G.end(); it++) {
            graphStatus[it.key()]["arrivedTime"] = { MAX, MAX, MAX };
            graphStatus[it.key()]["totalPrice"] = MAX;
            graphStatus[it.key()]["scheduleList"] = {}; // Price and Time
        }
        graphStatus[from]["arrivedTime"] = startTime;
        graphStatus[from]["totalPrice"] = 0;

        map<string, bool> viaMap;
        for (string city : via) {
            viaMap[city] = false;
        }
        //进行 A-star 优化过的深搜
        adfsAmongCities(from, to, viaMap, timeLimit, moneyLimit, priority, vis, graphStatus, plan, final_plan);
        final_plan["scheduleList"] = chronologize(final_plan["scheduleList"], startTime);
        return final_plan;
    }

    bool checkVia(const map<string, bool>& via)
    {
        for (auto it : via) {
            if (it.second == false) {
                return false;
            }
        }
        return true;
    }

    int Astar_f_among(int g, const string& from, const map<string, bool>& via, const string& to)
    { // Astar 中的启发函数f(n) = g(n) + h(n)
        int distance = 0;
        int value = 0;
        if (!checkVia(via)) {
            for (auto it : via) {
                distance += int(Distance[from][it.first]);
            }
            value = distance * FARE;

        } else {
            distance = int(Distance[from][to]);
            value = g + distance * FARE;
        }
        return value;
    }

    void
    adfsAmongCities(const string& currentFrom,
        const string& finallyTo,
        map<string, bool> via,
        const vector<int>& timeLimit,
        const int& moneyLimit,
        PRIORITY& priority,
        map<string, bool>& vis,
        json& graphStatus,
        json& plan,
        json& final_plan)
    {
        if (currentFrom == finallyTo && checkVia(via)) { // 如果抵达终点
            if (final_plan["totalPrice"].is_null() /*第一个抵达终点的计划*/
                || (priority == MONEY && plan["totalPrice"] < final_plan["totalPrice"]) /*总价比先前计划要低的计划*/
                || (priority == TIME && plan["arrivedTime"] < final_plan["arrivedTime"]) /*抵达时间比先前计划要早的计划*/) {
                final_plan = plan;
            }
        } else {
            // 建立优先队列并计算每一个行程的启发函数值
            priority_queue<json, vector<json>, cmp_schedule_Astar_f_increase> schedule_pq;
            for (json schedule : G[currentFrom]) {
                int schedule_price = int(schedule["price"]);
                string schedule_to = schedule["to"];
                schedule["Astar_f"] = Astar_f_among(schedule_price, schedule_to, via, finallyTo);
                schedule_pq.push(schedule);
            }

            int currentFromAstar_f = Astar_f_among(plan["totalPrice"], currentFrom, via, finallyTo);
            currentFromAstar_f *= RELAXION_AMONG_CITY; // 松弛操作，留有余地。后文解释为什么

            // 遍历每一个行程
            while (!schedule_pq.empty()) {
                json schedule = schedule_pq.top();
                schedule_pq.pop();

                if (!vis[schedule["to"]]) { // 如果该行程的目的地在当前 plan 中还未经过，则尝试

                    string schedule_to = schedule["to"]; // 之所以这样倒腾来倒腾去是为类型转换 // 之所以这样倒腾来倒腾去是因为当时不知道有get<string>()这个直接从 json 中读数据的函数。不过现在这样临时取出来应该也能加快速度，毕竟少了一次调度
                    string schedule_from = schedule["from"];

                    vis[schedule_to] = true; // 加入到已抵达城市集合
                    plan["scheduleList"].push_back(schedule); // 将该行程加入到plan中
                    plan["totalPrice"] = int(plan["totalPrice"]) + int(schedule["price"]);
                    vector<int> arrivedTime = addtime_by_schedule(plan["arrivedTime"], schedule);
                    vector<int> previousArrivedTime = plan["arrivedTime"];
                    plan["arrivedTime"] = arrivedTime;
                    /*among*/ if (via.count(schedule_to))
                        via[schedule_to] = true;

                    // c++语言技术上的问题：因为json键名需要是字符串，故这里需要用某种方式将整形转换为字符串
                    // stringstream sstr;
                    // int temp = schedule["price"];
                    // temp /= 100;
                    // sstr << temp;
                    // string tempPriceStr = sstr.str();

                    // 我快感动哭了。c++库居然带了 int 转换成 string。上面那个建立输入输出流可浪费了好多性能啊...
                    string tempPriceStr = to_string(schedule["price"].get<int>() / 100);
                    //                      不对。为什么使用了自带的库函数，速度反而慢到令人发指呢

                    // 第一次剪枝：剪同样价格但不同时间的航班
                    int ifBetter = 0;
                    // 更新同价格但抵达时间不同的航班的最早抵达时间
                    if (graphStatus[schedule_to]["scheduleList"][tempPriceStr].is_null() || plan["arrivedTime"] < graphStatus[schedule_to]["scheduleList"][tempPriceStr]) {
                        graphStatus[schedule_to]["scheduleList"][tempPriceStr] = plan["arrivedTime"];
                    }
                    if (plan["arrivedTime"] <= graphStatus[schedule_to]["scheduleList"][tempPriceStr] /*对同一种航班剪枝*/
                        && arrivedTime <= timeLimit && plan["totalPrice"] <= moneyLimit) {
                        ++ifBetter;
                    }

                    // 第二次剪枝：剪远离目的地的行程
                    if (currentFromAstar_f > schedule["Astar_f"]) { // 启发函数下降，说明离搜索结果更近了。而之前的松弛操作是为了允许远离目的地一点点以保证搜索出最优解
                        ++ifBetter;
                    }

                    // 第 n 次剪枝：其余剪枝技术

                    if (ifBetter == 2) {
                        // 最后一次剪枝:如果还没有一个抵达目的地的可行计划，则不剪枝（因为没有参考对象）。如果有，则将比较当前花费与一个可行计划比较。在一个计划在没有抵达目的地的情况下花费已经比一个可行计划多时，舍之
                        // /*观察*/ cout << schedule["from"] << schedule["to"] << schedule["Astar_f"] << " " << plan["totalPrice"] << " " << json(arrivedTime) << " " << moneyLimit << " " << json(timeLimit) << final_plan["totalPrice"] << endl;
                        if (final_plan["totalPrice"].is_null()) {
                            adfsAmongCities(schedule_to, finallyTo, via, timeLimit, moneyLimit, priority, vis, graphStatus, plan, final_plan);
                        } else if ((priority == MONEY && plan["totalPrice"] < final_plan["totalPrice"]) /*总价比先前计划要低的计划*/
                            || (priority == TIME && plan["arrivedTime"] < final_plan["arrivedTime"]) /*抵达时间比先前计划要早的计划*/) {
                            adfsAmongCities(schedule_to, finallyTo, via, timeLimit, moneyLimit, priority, vis, graphStatus, plan, final_plan);
                        }
                    }

                    // 已经尝试过这条路径，将尝试所留下的痕迹抹除，所有信息恢复到尝试该路径之前的状态。准备尝试下一个行程
                    plan["arrivedTime"] = previousArrivedTime;
                    plan["totalPrice"] = int(plan["totalPrice"]) - int(schedule["price"]);
                    plan["scheduleList"].erase(plan["scheduleList"].end() - 1);
                    vis[schedule_to] = false;
                    /*among*/ if (via.count(schedule_to))
                        via[schedule_to] = false;
                }
            }
        }
    }
};

// int main()
// {
//     cout << "编译完成" << endl;
//     string from = "北京";
//     string to = "广州";
//     vector<string> via = { "上海" };
//     vector<int> startTime = { 0, 0, 0 };
//     vector<int> timeLimit = { 10, 0, 0 };
//     int moneyLimit = 100000;
//     PRIORITY priority = MONEY;

//     AstarDFS Astar;

//     json plan;
//     plan = Astar.ADFSAmongCities(from, to, via, startTime, timeLimit, moneyLimit, MONEY);
// }