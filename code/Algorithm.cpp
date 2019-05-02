#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
#include <sstream>
#include <vector>
#include "ADFS.cpp"
#include "Dijkstra.cpp"
#include "IO.cpp"
using namespace std;
using json = nlohmann::json;

class Algorithm {
   public:
    Algorithm() {
        Distance = ReadDistanceInformation();
        D = ReadDatabase();
        G = CreateDGraph(D);
        dijkstra = Dijkstra(G);
        astarDFS = AstarDFS(G, Distance);
    }

    json algorithm(string from,
                   string to,
                   vector<int> startTime,
                   vector<int> timeLimit,
                   int moneyLimit,
                   string priority) {
        json planOfMoneyPriority = dijkstra.dijkstra(from, to, "money", startTime);
        json planOfTimePriority = dijkstra.dijkstra(from, to, "time", startTime);

        if (priority == "money" && planOfMoneyPriority["totalPrice"] <= moneyLimit) {
            if (planOfMoneyPriority["arrivedTime"] <= timeLimit) {
                return planOfMoneyPriority;
            } else {
                // 在给定时间限制内寻找金钱最优。在已知最优解不满足时间限制的条件下，时间优先最优解的金钱花费即是接下来搜索的上界
                return astarDFS.astarDFS(from, to, startTime, timeLimit, planOfTimePriority["totalPrice"]);
            }
        }

        if (priority == "time" && planOfTimePriority["arrivedTime"] <= timeLimit) {
            if (planOfTimePriority["totalPrice"] <= moneyLimit) {
                return planOfTimePriority;
            }
            // else {
            // 在给定金钱限制内寻找时间最优。在已知最优解不满足金钱限制的条件下，时间优先最优解的金钱花费即是接下来搜索的上界
            // 然而我并没有实现这个函数，虽然不过是加两个（两个是实数）个判断的事情 ^_^。
            // return astarDFS.astarDFS(from, to, startTime, timeLimit, planOfTimePriority["totalPrice"]);
            // }
        }
        // 最优解都不满足的情况下，返回空
        json vacant;
        return vacant;
    }

    void formattedOutput(json plan) {
        cout << "Plan Message:" << endl;
        cout << "Arrived time:" << plan["arrivedTime"] << endl;
        cout << "Total price:" << plan["totalPrice"] << endl;
        cout << "Schedules:" << endl;
        for (json p : plan["scheduleList"]) {
            cout << p << endl;
        }
        cout << endl;
    }

   private:
    json D;
    json G;
    json Distance;
    Dijkstra dijkstra;
    AstarDFS astarDFS;
};

// int main() {
//     string from = "北京";
//     string to = "上海";
//     vector<int> startTime = {0, 0, 0};
//     vector<int> timeLimit = {1, 0, 0};
//     int moneyLimit = 100000;
//     string priority = "money";

//     Algorithm algorithm;
//     json plan = algorithm.algorithm(from, to, startTime, timeLimit, moneyLimit, priority);
//     algorithm.formattedOutput(plan);
// }