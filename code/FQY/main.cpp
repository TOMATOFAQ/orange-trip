#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
using namespace std;
using json = nlohmann::json;

int main() {
    json t;
    t["list"] = {};
    t["list"].push_back("xixi");
    t["list"].erase(t["list"].end() - 1);

    t["p"] = 0;
    t["p"] = t["p"] + 5;

    cout << t;

    return 0;
}
