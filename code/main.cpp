#include <unistd.h>
#include <ctime>
#include <iostream>
#include "Dijkstra.cpp"
using namespace std;

int Shutdown = 0;

// 时钟模块
// TODO: 加一个命名空间
time_t realtime = time(nullptr);
int mytime = int(realtime);
int multiple = 1;

void SetTimeSpeed(int m) { multiple = m; }

int GetTime() { return mytime; }

void* MoveTime(void* args) {  // 为什么形参列表要加void* args？
    while (true) {
        mytime += multiple;
        if (multiple) cout << "current time:" << mytime << endl;
        sleep(1);
        if (Shutdown) pthread_exit(NULL);
    }
}

void strategy_selection() {
    cout << "Please input the name of 'from':";
    string from;
    cin >> from;
    cout << "Please input the name of 'to':";
    string to;
    cin >> to;
    int strategy_select;  //
    cout << "Please select your strategy, money fitst or time first? (0/1):";
    cin >> strategy_select;

    vector<int> t = {0, 0, 0};
    json to_status;
    to_status = dijkstra<struct cmp_schedule_time_increase>(from, to, t);
    cout << to_status;
}

// 控制台模块
// TODO: 加命名空间
// TODO: 如何对一次算法新建一个线程，运算完毕之后即退出线程？
// TODO:
// 写一个函数，参数是函数指针，将各个选项传入。目的是将时间停止的代码复用。
void* control(void* args) {
    while (true) {
        cout << "Please input your command:" << endl;
        int command;
        cin >> command;
        switch (command) {
            case 0:
                Shutdown = 1;
                break;
            case 1:
                int m;
                cout << "Please in put mutiple:";
                cin >> m;
                SetTimeSpeed(m);
                break;
            case 2: {
                int pre_multiple = multiple;
                SetTimeSpeed(0);
                strategy_selection();
                SetTimeSpeed(pre_multiple);
            } break;
            default:;
        }
        if (Shutdown) pthread_exit(NULL);
    }
}

int main() {
    pthread_t tids[2];  // 新建进程
    int pM = pthread_create(&tids[1], NULL, control, NULL);
    int pT = pthread_create(&tids[0], NULL, MoveTime, NULL);
    pthread_exit(NULL);
}
