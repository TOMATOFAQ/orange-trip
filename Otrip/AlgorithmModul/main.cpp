#include "system.cpp"
#include <iostream>
#include <string>
#include <vector>
// 接口函数
using namespace std;
int main(int argc, char* argv[])
{
#define release 1
#if release
    string from = argv[1];
    string to = argv[2];
    std::vector<string> via = {};
    for (int i = 3; i < argc - 7 - 1; i++)
        via.push_back(argv[i]);
    std::vector<int> startTime;
    startTime.push_back(atoi(argv[argc - 7 - 1]));
    startTime.push_back(atoi(argv[argc - 6 - 1]));
    startTime.push_back(atoi(argv[argc - 5 - 1]));
    std::vector<int> timeLimit;
    timeLimit.push_back(atoi(argv[argc - 4 - 1]));
    timeLimit.push_back(atoi(argv[argc - 3 - 1]));
    timeLimit.push_back(atoi(argv[argc - 2 - 1]));
    int moneyLimit = atoi(argv[argc - 1 - 1]);
    string priority = argv[argc - 1];
    PRIORITY intPriority = (priority == "money") ? MONEY : TIME;
#else
    cout << "编译成功" << endl;
    std::string from = "北京";
    std::string to = "广州";
    std::vector<string> via = {};
    std::vector<int> startTime = { 1, 0, 0 };
    std::vector<int> timeLimit = { 2, 0, 0 };
    int moneyLimit = 1000;
    int intPriority = MONEY;
#endif

    /* 请使用gcc -O3 优化等级进行编译！！不过貌似-O2更快一些？
cd "/Users/samuel/Desktop/OrangeTravel/AlgorithmModul/" && g++ -O3 main.cpp -o main && "/Users/samuel/Desktop/OrangeTravel/AlgorithmModul/"main
cd "/Users/samuel/Desktop/OTrip/AlgorithmModul/" && g++ -O3 main.cpp -o main && "/Users/samuel/Desktop/OTrip/AlgorithmModul/"main
    */

    System system;
    json plan = system.algorithmModul(from, to, via, startTime, timeLimit, moneyLimit, intPriority);
    std::cout << plan;
}