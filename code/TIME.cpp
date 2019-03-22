#include <iostream>
#include <ctime>
#include <pthread.h>
#include <unistd.h>
using namespace std;

time_t realtime = time(nullptr);
int mytime = int(realtime);
int multiple = 1;

void SetTimeSpeed(int m) {multiple = m;}

int GetTime() {return mytime;}

void* MoveTime(void* args){ // 为什么形参列表要加void* args？
    while(true){
        mytime += multiple;
        sleep(1);
    }
}

// 新建一个进程
pthread_t tids[1];
int ret = pthread_create(&tids[0],NULL, MoveTime ,NULL);
// pthread_exit(NULL);
