#include "ADFS.cpp"
#include "Dijkstra.cpp"
#include "IO.cpp"
#include "json.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <sstream>
#include <vector>
#define PRIORITY int
#define MONEY 1
#define TIME 2
using namespace std;
using json = nlohmann::json;

class Algorithm {
public:
    // private:
    json D;
    json G;
    json Distance;
    Dijkstra dijkstra;
    AstarDFS astarDFS;

public:
    Algorithm()
    {
        Distance = ReadDistanceInformation();
        D = ReadDatabase();
        G = CreateDGraph(D);
        dijkstra = Dijkstra(G);
        astarDFS = AstarDFS(G, Distance);
    }

    static void formattedOutput(json& plan)
    {
        cout << "Plan Message:" << endl;
        cout << "Arrived time:" << plan["arrivedTime"] << endl;
        cout << "Total price:" << plan["totalPrice"] << endl;
        cout << "Schedules:" << endl;

        for (json s : plan["scheduleList"]) {
            cout << s << endl;
        }

        cout << endl;
    }

    json searchPathBetweenCities(string from,
        string to,
        vector<int> startTime = { 0, 0, 0 },
        vector<int> timeLimit = { MAX, MAX, MAX },
        int moneyLimit = MAX,
        PRIORITY priority = MONEY)
    {
        json planOfMoneyPriority = dijkstra.dijkstra(from, to, MONEY, startTime);
        json planOfTimePriority = dijkstra.dijkstra(from, to, TIME, startTime);
        json finalPlan;

        if (priority == MONEY && planOfMoneyPriority["totalPrice"] <= moneyLimit) {
            if (planOfMoneyPriority["arrivedTime"] <= timeLimit) {
                finalPlan = planOfMoneyPriority;
            } else {
                // 在给定时间限制内寻找金钱最优。在已知最优解不满足时间限制的条件下，时间优先最优解的金钱花费即是接下来搜索的上界
                finalPlan = astarDFS.astarDFS(from, to, startTime, timeLimit, planOfTimePriority["totalPrice"], priority);
            }
        }

        if (priority == TIME && planOfTimePriority["arrivedTime"] <= timeLimit) {
            if (planOfTimePriority["totalPrice"] <= moneyLimit) {
                finalPlan = planOfTimePriority;
            } else {
                // 在给定金钱限制内寻找时间最优。在已知最优解不满足金钱限制的条件下，金钱优先最优解的时间花费即是接下来搜索的上界
                finalPlan = astarDFS.astarDFS(from, to, startTime, planOfMoneyPriority["arrivedTime"], moneyLimit, priority);
            }
        }

        // 最优解都不满足的情况下，返回空
        return finalPlan;
    }

    // 其实这里打了快 300 行。然后决定放弃做有时间限制同时有必经城市限制的算法，因为意识到需要写出栈入栈形式的 dfs，遂放弃
    json searchPathAmongCities(string from,
        string to,
        vector<string> via,
        vector<int> startTime = { 0, 0, 0 },
        vector<int> timeLimit = { MAX, MAX, MAX },
        int moneyLimit = MAX,
        PRIORITY priority = MONEY)
    {

        json planOfMoneyPriority = dijkstra.dijkstraSearchPathAmongCities(from, to, via, startTime, MONEY);
        json planOfTimePriority = dijkstra.dijkstraSearchPathAmongCities(from, to, via, startTime, TIME);
        json finalPlan;

        if (priority == MONEY && planOfMoneyPriority["totalPrice"] <= moneyLimit) {
            if (planOfMoneyPriority["arrivedTime"] <= timeLimit) {
                finalPlan = planOfMoneyPriority;
            } else {
                // 在给定时间限制内寻找金钱最优。在已知最优解不满足时间限制的条件下，时间优先最优解的金钱花费即是接下来搜索的上界
                finalPlan = astarDFS.aStarDFSAmongCities(from, to, via, startTime, timeLimit, planOfTimePriority["totalPrice"], priority);
            }
        }

        if (priority == TIME && planOfTimePriority["arrivedTime"] <= timeLimit) {
            if (planOfTimePriority["totalPrice"] <= moneyLimit) {
                finalPlan = planOfTimePriority;
            } else {
                // 在给定金钱限制内寻找时间最优。在已知最优解不满足金钱限制的条件下，金钱优先最优解的时间花费即是接下来搜索的上界
                finalPlan = astarDFS.aStarDFSAmongCities(from, to, via, startTime, planOfMoneyPriority["arrivedTime"], moneyLimit, priority);
            }
        }

        // 最优解都不满足的情况下，返回空
        return finalPlan;
    }
};

// int main()
// {
//     cout << "编译完成" << endl;
//     string from = "北京";
//     string to = "上海";
//     vector<string> via = { "天津", "郑州", "拉萨" };
//     vector<int> startTime = { 0, 0, 0 };
//     vector<int> timeLimit = { 10, 0, 0 };
//     int moneyLimit = 13600;
//     PRIORITY priority = MONEY;

//     Algorithm algorithm;
//     json plan;
//     plan
//         = algorithm.searchPathAmongCities(from, to, via, startTime, timeLimit, moneyLimit, MONEY);
//     algorithm.formattedOutput(plan);
//     plan = algorithm.searchPathAmongCities(from, to, via, startTime, timeLimit, moneyLimit, TIME);
//     algorithm.formattedOutput(plan);

// plan = algorithm.astarDFS.aStarDFSAmongCities(from, to, via, startTime, timeLimit, moneyLimit, MONEY);
//     algorithm.formattedOutput(plan);
// }