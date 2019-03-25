#include <pthread.h>
#include <unistd.h>
#include <ctime>
#include <iostream>
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
        sleep(1);
        if (Shutdown) pthread_exit(NULL);
    }
}

// 控制台模块
// TODO: 也是要加一个命名空间
// TODO: 如何对一次算法新建一个线程，运算完毕之后即退出线程？
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
                cout << GetTime() << endl;
                break;
            case 2:
                cin >> command;
                SetTimeSpeed(command);
                break;
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
