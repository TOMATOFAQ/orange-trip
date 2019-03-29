# DFS模块

DFS 模块只实现了查找最少金钱消费策略。由于该模块过于简单，所以不做详细说明。

DFS 中对搜索树进行了剪枝。方法是，时刻判断当前路径的消费是否大于可行路径优先队列中的最劣解，一旦超过，终止该枝的后续运算。可行路径优先队列以消费的降序排列，消费的值大被弹出的优先级高。这样设置优先级的目的是为了获取一组可行解而非一个。

# 函数定义
```cpp

struct cmp_plan_decrease {  // 优先队列的比较函数，总价小的被pop 掉的优先级高
    bool operator()(json a, json b) {
        return a["total_price"] < b["total_price"];  // 大的优先级高
    }
};

struct cmp_plan_increase {  // 优先队列的比较函数，总价大的被pop 掉的优先级高
    bool operator()(json a, json b) {
        return a["total_price"] > b["total_price"];  // 小的优先级高
    }
};

struct cmp_schedule_increase {  // 优先队列的比较函数，总价小的被pop
                                // 掉的优先级高
    bool operator()(json a, json b) {
        return a["price"] > b["price"];  // 小的优先级高
    }
};

struct cmp_schedule_decrease {  // 优先队列的比较函数，总价大的被pop
                                // 掉的优先级高
    bool operator()(json a, json b) {
        return a["price"] < b["price"];  // 大的优先级高
    }
};

void dfs(priority_queue<json, vector<json>, cmp_plan_decrease> &ps, json &p,
         vector<string> &have_been_to, json schedule, string start,
         string end);

priority_queue<json, vector<json>, cmp_plan_decrease> DFS(string start,
                                                          string end);

// // 使用说明
// int main() {
//     ReadDatabase();
//     cout << "系统加载完成。" << endl;
//     priority_queue<json, vector<json>, cmp_plan_decrease> ps =
//         DFS("Beijing", "Shenzhen");
//     while (!ps.empty()) {
//         json p = ps.top();
//         ps.pop();
//         cout << p["total_price"] << endl;
//         for (json s : p["path"]) cout << s << endl;
//         cout << endl;
//     }

//     // 剪枝后北京到深圳可以很容易求出来了，而北京到东京依然是组合爆炸。
//     return 0;
// }

// // 413
// //
// {"data":[0,11,40],"duration":[0,5,47],"end":[0,17,27],"from":"Beijing","number":"Z35","price":93,"start":[0,11,40],"to":"Zhengzhou","type":"HSR"}
// //
// {"data":[1,9,40],"duration":[0,15,0],"end":[1,0,40],"from":"Zhengzhou","number":"7719","price":320,"start":[0,9,40],"to":"Shenzhen","type":"N"}

// // 413
// //
// {"data":[0,11,40],"duration":[0,5,47],"end":[0,17,27],"from":"Beijing","number":"Z35","price":93,"start":[0,11,40],"to":"Zhengzhou","type":"HSR"}
// //
// {"data":[1,11,0],"duration":[0,15,0],"end":[1,2,0],"from":"Zhengzhou","number":"7715","price":320,"start":[0,11,0],"to":"Shenzhen","type":"N"}

// // 413
// //
// {"data":[0,13,0],"duration":[0,5,41],"end":[0,18,41],"from":"Beijing","number":"Z97","price":93,"start":[0,13,0],"to":"Zhengzhou","type":"HSR"}
// //
// {"data":[1,9,40],"duration":[0,15,0],"end":[1,0,40],"from":"Zhengzhou","number":"7719","price":320,"start":[0,9,40],"to":"Shenzhen","type":"N"}

```