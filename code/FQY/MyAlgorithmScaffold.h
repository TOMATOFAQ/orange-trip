#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
#include <vector>
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