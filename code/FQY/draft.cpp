#include <iostream>
#include <nlohmann/json.hpp>
#include <queue>
#include <set>
#include <vector>
using json = nlohmann::json;
using namespace std;

int main() {
    json j;
    j["t"] = {0, 19, 35};
    json k;
    k["t"] = {0, 19, 50};

    cout << (j["t"] > k["t"]);
}
