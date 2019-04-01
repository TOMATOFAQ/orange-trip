#define CITYNUM 12

#include <algorithm>
#include <map>
#include <utility>
#include <vector>

std::vector<bool> known(CITYNUM,
                        false);  //标记每个点是否被访问过,初始化：都没被访问过
std::multimap<int, schedule> database;

if (strategy == 2)  //第三问
{
    //初始化每个点的到达时间为MAX
    std::vector<vector<int>> tempTime(CITYNUM, {MAX, MAX, MAX});
    //抵达每个点时的 花费
    std::vector<int> tempValue(CITYNUM);
    //记录抵达每个节点所经过的 schedule 序列
    std::vector<schedule> path;

    tempTime[from] = startTime;  //对应这句话 status[from]["t"] = t;

    DFS(from, path, known, tempTime, tempValue);
}

//递归求路径
// city: 城市编号
// destination:目的城市编号
void DFS(int city, std::vector<schedule>& path, std::vector<bool>& known,
         std::vector<vector<int>>& tempTime, std::vector<int>& tempValue) {
    //对策略三进行剪枝
    if (strategy == 2 &&
        (tempTime[city] > deadlineTime ||
         tempValue[city] > min))  //总时间大于截至时间，不满足约束条件
        return;

    known[city] = true;  //标记此城市已访问过

    //已经递归到目的地城市
    if (city == destination) {
        int ok = true;

        // throughCity:vector<bool>,储存用户选择的必经城市
        std::vector<bool> mark = throughCity;

        //将路径上的所有城市取消标志
        for (std::vector<schedule>::size_type ix = 0; ix != path.size(); ix++)
            mark[path[ix]["to"]] = false;
        mark[from] = false;

        //若必经城市还有点未取消标志，即必经城市未被完全经过，不合用户要求
        // isChecked:bool,判断用户是否选择必经城市
        if (isChecked) {
            for (std::vector<bool>::size_type ix = 0; ix != mark.size(); ix++) {
                if (mark[ix] == true) {
                    ok = false;
                    break;
                }
            }
        }

        //若满足约束条件，则更新花费最小值min，到达每个节点的时间序列time，并记录路径plan
        if (strategy = 2 && tempValue[city] < min && ok) {
            min = tempValue[city];
            time = tempTime;
            plan = path;
        }
    }
    //未到达目的地城市，继续向下一层递归
    else {
        typedef std::multimap<int, schedule>::size_type
            sz_type;  //#include <map>

        sz_type entries = Schedule::database.count(city);

        std::multimap<int, schedule>::iterator iter =
            Schedule::database.find(city);
        std::multimap<int, schedule>::iterator min = iter;
        bool start = true;

        for (sz_type cnt = 0; cnt != entries; cnt++, iter++) {
            if (known[(iter->second)["to"]] ==
                true)  //如果去往城市已经访问过，则忽略该路径
            {
                if (start) min++;
                continue;
            }
            start = false;

            //策略三不能局部最优剪枝，所以每条路径都需要尝试
            {
                path.push_back(iter->second);

                //更新时间和花费
                tempTime[(iter->second)["to"]] =
                    CalculateTime(iter, tempTime);  //计算时间
                tempValue[(iter->second)["to"]] =
                    tempValue[city] + (iter->second)[price];

                DFS((iter->second)["to"], path, known, tempTime, tempValue);

                known[(iter->second)["to"]] = false;
                path.erase(path.end());
            }
        }
    }
}