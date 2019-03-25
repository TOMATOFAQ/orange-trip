#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
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

void dfs(vector<vector<json>> &ps, vector<json> &p,
         vector<string> &have_been_to, json schedule, string start,
         string end) {
    if (start == end) {
        ps.push_back(p);
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
                p.push_back(s);
                dfs(ps, p, have_been_to, s, s["to"], end);
                p.pop_back();
                have_been_to.pop_back();
            }
        }
    }
}

vector<vector<json>> DFS(string start, string end) {
    vector<vector<json>> ps;
    vector<json> p;
    vector<string> have_been_to{"Beijing"};
    dfs(ps, p, have_been_to, NULL, start, end);
    return ps;
}

int main() {
    ReadDatabase();

    vector<vector<json>> ps = DFS("Beijing", "Shanghai");

    for (vector<json> &p : ps) {
        for (json s : p) {
            cout << s << endl;
        }
        cout << endl;
    }

    // 找出所有路径会出现组合爆炸的情况，在当前数据集不加限制不可能跑得完。
    return 0;
}
