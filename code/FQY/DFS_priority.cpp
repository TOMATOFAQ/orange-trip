#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <vector>
using namespace std;
using json = nlohmann::json;

vector<json> D;
void ReadDatabase() {
    string line;
    ifstream infile;
    infile.open("database.txt");
    while (getline(infile, line)) {
        json j = json::parse(line);
        D.push_back(j);
    }
}

struct cmp_plan {  // 优先队列的比较函数，总价高的被pop 掉的优先级高
    bool operator()(json a, json b) {
        return a["total_price"] < b["total_price"];  // 大的优先级高
    }
};

void dfs(priority_queue<json, vector<json>, cmp_plan> &ps, json &p,
         vector<string> &have_been_to, json schedule, string start,
         string end) {
    if (start == end) {
        if (ps.size() < 5)
            ps.push(p);
        else {
            ps.pop();
            ps.push(p);
        }
    } else {
        vector<json> temp;
        for (json s : D) {
            if (s["from"] == start) {
                temp.push_back(s);
            }
        }
        for (json s : temp) {
            if (!count(have_been_to.begin(), have_been_to.end(), s["to"])) {
                have_been_to.push_back(s["to"]);
                p["path"].push_back(s);
                p["total_price"] = int(p["total_price"]) + int(s["price"]);
                // 为什么这里需要类型转换？
                if (!ps.size()) {
                    dfs(ps, p, have_been_to, s, s["to"], end);
                }  // 剪枝
                else if (p["total_price"] < ps.top()["total_price"]) {
                    dfs(ps, p, have_been_to, s, s["to"], end);
                }
                p["total_price"] = int(p["total_price"]) - int(s["price"]);
                p["path"].erase(p["path"].end() - 1);
                have_been_to.pop_back();
            }
        }
    }
}

priority_queue<json, vector<json>, cmp_plan> DFS(string start, string end) {
    priority_queue<json, vector<json>, cmp_plan> ps;
    json p;
    p["path"] = {};
    p["total_price"] = 0;
    vector<string> have_been_to{"Beijing"};
    dfs(ps, p, have_been_to, NULL, start, end);
    return ps;
}

int main() {
    ReadDatabase();

    priority_queue<json, vector<json>, cmp_plan> ps =
        DFS("Beijing", "Shenzhen");
    while (!ps.empty()) {
        json p = ps.top();
        ps.pop();
        cout << p["total_price"] << endl;
        for (json s : p["path"]) cout << s << endl;
        cout << endl;
    }

    // 剪枝后北京到深圳可以很容易求出来了，而北京到东京依然是组合爆炸。

    return 0;
}
