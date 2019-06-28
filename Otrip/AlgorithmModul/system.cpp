//
// Created by Samuel on 2019-05-03.
//
#include "Algorithm.cpp"

class System {
public:
    json algorithmModul(const string from,
        const string to,
        const vector<string> via,
        const vector<int> startTime,
        const vector<int> timeLimit,
        int moneyLimit,
        const PRIORITY priority)
    {
        json finalPlan;
        json empty;

        if (via.empty()) {
            finalPlan = algorithm.searchPathBetweenCities(from, to, startTime, timeLimit, moneyLimit, priority);
        } else {
            finalPlan = algorithm.searchPathAmongCities(from, to, via, startTime, timeLimit, moneyLimit, priority);
        }
        return (finalPlan["totalPrice"] <= moneyLimit && finalPlan["arrivedTime"] <= timeLimit) ? finalPlan : empty;
    }

private:
    Algorithm algorithm;
};
