import json
import random
max = 99999999

'''数据准备'''

# schedule 的线性表
D = []
with open("database.txt") as f:
    for l in f:
        l = l.replace("'", '"')
        D.append(json.loads(l))

# directed graph
G = {}
for s in D:
    try:
        G[s["from"]].append(s)
    except:
        G[s["from"]] = []

# 城市名列表
V = list(G.keys())

# 按键索引的距离“矩阵”
Dis = {}  # 一个粗略到无以复加的相邻节点的距离矩阵
for s in range(len(V)):
    Dis[V[s]] = {}
    for t in range(len(V)):
        Dis[V[s]][V[t]] = 0
        for schedule in G[V[s]]:
            if schedule["to"] == V[t]:
                time = schedule["duration"][0] * 24 + \
                    schedule["duration"][1] + schedule["duration"][2]/60.0
                speed = 100 if(schedule["type"] == "N")else 300 if(
                    schedule["type"] == "HSR")else 900
                Dis[V[s]][V[t]] = time*speed


'''蚁群算法'''

# 参数定义
''' 
ALPHA:信息启发因子，值越大，则蚂蚁选择之前走过的路径可能性就越大，值越小，则蚁群搜索范围就会减少，容易陷入局部最优
BETA:Beta值越大，蚁群越就容易选择局部较短路径，这时算法收敛速度会加快，但是随机性不高，容易得到局部的相对最优
RHO:全局信息挥发因子
Q:信息量增量参数
'''
(ALPHA, BETA, RHO, Q) = (1.0, 2.0, 0.5, 100.0)
(city_num, ant_num) = (len(V), 50)


class Ant:

    def __init__(self, ID, source, destination):
        self.ID = ID
        self.source = source
        self.destination = destination

        self.plan = []
        self.visited_cities = []
        self.current_city = source
        # availiable_cities 为实际可访问城市。即可访问节点与访问过节点的差集
        self.availiable_cities = [schedule['to']
                                  for schedule in G[self.source]]
        self.move_count = 0

    def __choice_next_city(self):
        availiable_cities_prob = [0.0 for schedule in in G[self.source]]
        total_prob = 0
        for city in availiable_cities:
            prob = pow(pheromone_graph[self.current_city][i], ALPHA) * \
                pow((1.0/distance_graph[self.current_city][i]), BETA)
            availiable_cities_prob.append(prob)
            total_prob += prob

        # 依据概率选择城市
        next_city = ""
        temp_prob = random.uniform(0.0, total_prob)
        for i in range(len(availiable_cities_prob)):
            temp_prob -= availiable_cities_prob[i]
            if temp_prob < 0:
                next_city = availiable_cities[i]
                break

        # 无路可走时返回空
        return next_city

    def __move(self):
        next_city = __choice_next_city()
        # 打到这里突然发现，不行。他们的信息素都是记录在节点上的。
        # 这个是多重图上的最短路径问题！需要把信息素记录在边上！
        # 还必须针对这个问题优化蚁群算法。
        # 你妈的，为什么这个大作业这么险恶？！越做越多的？
