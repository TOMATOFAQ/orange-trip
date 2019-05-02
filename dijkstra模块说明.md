# Dijkstra模块

该模块的主函数为template <typename T>
json dijkstra(string from, string to, vector<int> t);。其功能为通过判断所传入的比较函数的类型，按比较函数求相应要求的行程表schedule list。返回值为一个抵达目标城市节点的状态，包括花费金钱，花费时间以及一个 schedule 序列。

# 变量定义

### 1. status的定义

status表示图上各个节点的到达信息。包括抵达时间t,抵达消费m,抵达所经过的 schedule 序列。
```js
json status;
status = {
    "Beijing":{
        "t":0,
        "m":0,
        "s":[]
    },
}
```

# 函数定义
```cpp
// 给当前时间 t1加上一段绝对时间t2
vector<int> addtime_absolutely(vector<int> t1, vector<int> t2);

// 结合当前时间，返回抵达时间
vector<int> addtime_by_schedule(vector<int> t, json s);

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
vector<int> addtime_by_schedule_by_start_time(vector<int> t, json s);
json chronologize(json sl);

//返回目标节点的status，包括花费金钱，花费时间以及 schedule 序列
template <typename T>
json dijkstra(string from, string to, vector<int> t);


// 使用说明
int main() {
    ReadDatabase();
    CreateDGraph();
    cout << "加载完毕。" << endl;

    vector<int> t = {0, 15, 44};
    json to_status;

    to_status =
        dijkstra<struct cmp_schedule_money_increase>("Beijing", "Shenzhen", t);
    for (json p : to_status) {
        cout << p << endl;
    }

    to_status =
        dijkstra<struct cmp_schedule_time_increase>("Beijing", "Shenzhen", t);
    for (json p : to_status) {
        cout << p << endl;
    }

    return 0;
}

/*
413
[{"data":[0,11,40],"duration":[0,5,47],"end":[0,17,27],"from":"Beijing","number":"Z35","price":93,"start":[0,11,40],"to":"Zhengzhou","type":"HSR"},{"data":[1,9,40],"duration":[0,15,0],"end":[1,0,40],"from":"Zhengzhou","number":"7719","price":320,"start":[0,9,40],"to":"Shenzhen","type":"N"}]
[3,0,40]

1849
[{"data":[0,17,0],"duration":[0,3,35],"end":[0,20,35],"from":"Beijing","number":"MF1749","price":1849,"start":[0,17,0],"to":"Shenzhen","type":"AC"}]
[0,20,35]

*/
```