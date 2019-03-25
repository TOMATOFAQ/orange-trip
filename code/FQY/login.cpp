#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>
using namespace std;
using json = nlohmann::json;

void GenerateExampleUser() {
    string username, pwd;
    ofstream outfile;
    outfile.open("users.txt");
    json U;
    U["name"] = "yj";
    U["pwd"] = "yj";
    outfile << U << endl;
}

int login(string name, string pwd) {
    ifstream infile;
    infile.open("users.txt");
    string line;
    int flag;
    while (getline(infile, line)) {
        json j = json::parse(line);
        if (j["name"] == name) {
            if (j["pwd"] == pwd)
                return 1;
            else
                return 0;
        }
    }
    return 0;
}

int main() {
    GenerateExampleUser();
    cout << login("yj", "yj");
    return 0;
}