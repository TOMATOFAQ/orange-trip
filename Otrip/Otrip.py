from os.path import abspath, dirname
import queue
from PyQt5.QtCore import QUrl, QObject, pyqtSlot, QFileInfo, pyqtSignal
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
import qrc  # 加载属性
from PyQt5 import QtCore, QtWidgets
from PyQt5 import QtQml, QtQuick
from PyQt5 import Qt
from PyQt5 import *
import sys
import threading
import time
import os
import json
import networkx as nx
import matplotlib
# matplotlib.use('TkAgg')

matplotlib.use("Qt5Agg", force=True)
# 抱着赴死的心态从头看如何将 matplotlib 画出的图传进 canvas 。把这句代码加进去之后，图弹出来的一瞬间整个人都震飞了，感觉是无数个光子穿透了我的身躯

# absPath = dirname(abspath(__file__)) + "/"
absPath = os.path.dirname(os.path.realpath(sys.argv[0])) + "/"
print(absPath)
os.chdir(absPath)


class Algorithm:
    def __init__(self):
        # 输入数据
        database = json.loads(open(absPath+"database.json", "r").read())
        distanceInformation = json.loads(
            open(absPath+"DistanceInformation.json", "r").read())

        # 生成图
        G = nx.Graph()
        cities = ['上海', '乌鲁木齐', '兰州', '北京', '南京', '南宁', '合肥', '呼和浩特', '哈尔滨', '天津', '太原', '广州', '成都',
                  '拉萨', '昆明', '杭州', '武汉', '济南', '海口', '石家庄', '福州', '西宁', '西安', '贵阳', '郑州', '重庆', '银川', '长春', '长沙']

        # 因为 networkx 这个库需要用数字来做下标，这里需建立每个城市的index
        index = []
        for i, _ in enumerate(cities):
            index.append(i)
        G.add_nodes_from(index)
        for s in database:
            G.add_edge(cities.index(s["from"]), cities.index(s["to"]))

        # 获取各个城市的位置信息
        pos = []
        for city in cities:
            pos.append(
                (distanceInformation[city]['经度'], distanceInformation[city]['纬度']))

        # 类属性
        self.cities = cities
        self.pos = pos
        self.G = G
        self.database = database
        self.distanceInformation = distanceInformation

    def search(self, start, to, via=[], startTime=[0, 0, 0], timeLimit=[999, 999, 999], moneyLimit=99999, priority="money",):
        # 生成命令
        main = absPath+'AlgorithmModul/main '
        command = main + " " + start + " " + to + " "

        for city in via:
            command += city + " "
        for i in startTime:
            command += str(i) + " "

        for i in timeLimit:
            command += str(i) + " "
        command += str(moneyLimit) + " "
        command += priority + " "
        print(command)
        # 执行命令并获得结果
        data = os.popen(command).readlines()

        if data:
            plan = data[0]
            plan = json.loads(plan)
        else:
            plan = None

        return plan

    def drawMap(self, plan="", schedule=""):
        # 画背景
        options_basic = {
            'pos': self.pos,
            'width': 1,
            'node_color': 'grey',
            'node_size': 50,
            'edge_color': 'grey'
        }
        nx.draw(self.G, **options_basic)

        # 画当前 plan
        if plan:
            path = [(s["from"], s["to"]) for s in plan["scheduleList"]]
            path_index = [(self.cities.index(pair[0]),
                           self.cities.index(pair[1])) for pair in path]
            node_index = set()
            for i in path_index:
                node_index.add(i[0])
                node_index.add(i[1])

            options_path = {
                'pos': self.pos,
                'width': 2,
                'node_color': 'black',
                'edge_color': 'black',
                'node_size': 50,
                'edgelist': path_index,
                'nodelist': node_index
            }
            nx.draw(self.G, **options_path)

        # 画当前schedule
        if schedule:
            schedule_edga_index = [
                (self.cities.index(schedule['from']), self.cities.index(schedule['to']))]
            schedule_node_index = [self.cities.index(
                schedule['from']), self.cities.index(schedule['to'])]
            options_schedule = {
                'pos': self.pos,
                'width': 3,
                'node_color': 'yellow',
                'edge_color': 'yellow',
                'node_size': 50,
                'edgelist': schedule_edga_index,
                'nodelist': schedule_node_index
            }
            nx.draw(self.G, **options_schedule)
        # 图片保存功能需要在环境中运行
        matplotlib.pyplot.title("详情")
        matplotlib.pyplot.show()  # 直接显示方式
        # plt.savefig('path.png') # 保存方式
        return 1


def formattedOutput(plan):
    print(plan["arrivedTime"])
    print(plan["totalPrice"])
    for s in plan['scheduleList']:
        print(s)


GTime = []  # 全局变量


class moveTimeClass(QObject):
    # 因为和界面交互的类必须继承自 QObject,所以必须在 Timer 外再创建一个类专门处理信号的交互
    timeChanged = pyqtSignal(str)

    def __init__(self):
        super(moveTimeClass, self).__init__()

    def run(self):
        global GTime
        self.timeChanged.emit(str(GTime))

    def moveTime(self):
        rootObject.synchroOperation(str(GTime))


moveTimeHandler = moveTimeClass()


class Timer (threading.Thread):  # 继承父类threading.Thread
    def __init__(self):
        threading.Thread.__init__(self)
        self.threadID = 1
        self.name = "Timer"
        self.counter = 1

        self.time = [
            0] + [int(i) for i in time.ctime(time.time()).split()[-2].split(":")[:2]]

        # 每秒走 1 分钟
        self.multiple = 1
        self.start()

    def run(self):  # 把要执行的代码写到run函数里面 线程在创建后会直接运行run函数
        while True:
            self.time[2] += self.multiple
            self.time[1] += self.time[2] // 60
            self.time[2] %= 60
            self.time[0] += self.time[1] // 24
            self.time[1] %= 24
            global GTime  # 声明全局变量
            GTime = self.time
            global moveTimeHandler
            moveTimeHandler.run()
            time.sleep(1)

    def changeSpeed(self):
        if not self.multiple:
            self.multiple = 1
        else:
            self.multiple *= 10
            self.multiple %= 1000

    def stopTime(self):
        self.multiple = 0
        return True

    def continueTime(self):
        self.multiple = 1
        return True

    def getTime(self):
        print(self.time)
        return self.time


class ComparableSchedule(object):
    # 由于系统内维护的优先队列需要以可比较的对象作为元素，故还需特意将一个 schedule 封装成一个类
    def __init__(self, schedule):
        self.schedule = schedule

    def __lt__(self, other):
        return self.schedule["date"] < other.schedule["date"]


class System(QObject):
    def __init__(self):
        super(System, self).__init__()
        # 初始化算法模块
        self.Algorithm = Algorithm()

        # 初始化日志记录模块
        json.dump({}, open("log.json", "w"))
        self.log = {}

        # 一个用以向客户端异步弹出信息的信号
        self.popMessageSignal = pyqtSignal(str)

        # 维护一个 scheduleQueue 列表，时事刷新是否该切换地点了
        self.scheduleQueue = queue.PriorityQueue()

        # 当前用户的位置：城市名/交通工具类型,用以显示及判别能否进行下一个schedule
        self.location = "北京"
        self.start = "北京"
        self.to = "北京"
        self.arrivedTime = [0, 0, 0]

    @pyqtSlot(result=bool)
    def popMessageHandler(self, message):
        rootObject.popMessage(str(message))
        return True

    @pyqtSlot(result=QVariant)
    def updateStatus(self):
        global GTime
        if GTime > self.arrivedTime:
            if self.location != self.to:
                self.lprint("用户抵达"+self.to)
            self.location = self.to
            self.start = self.to

        return {"location": self.location,
                "start": self.start,
                "to": self.to,
                "arrivedTime": self.arrivedTime}

    @pyqtSlot(result=QVariant)
    def updateScheduleQueue(self):
        # 更新一个表刷新 qml 中的listView
        if self.scheduleQueue.queue:
            global GTime
            while self.scheduleQueue.queue[0].schedule["date"] < GTime:
                schedule = self.scheduleQueue.get().schedule
                print("弹出并执行行程：", str(schedule))

                if schedule["from"] == self.location:
                    self.popMessageHandler("正在前往："+schedule["to"])
                    self.location = schedule["number"]
                    self.start = schedule["from"]
                    self.to = schedule["to"]
                    self.arrivedTime = schedule["arrivedTime"]
                else:
                    self.popMessageHandler(
                        "您错过了由"+schedule["from"]+"发往" + schedule["to"] + "的班车！")
                    self.lprint(
                        "用户错过了由"+schedule["from"]+"发往"+schedule["to"]+"的行程")

                if not self.scheduleQueue.queue:
                    break
        tempList = [cs.schedule for cs in self.scheduleQueue.queue]
        return tempList

    @pyqtSlot(str, str, str, str, str, int, int, int, int, int, int, result=QVariant)
    def receiveDemand(self, fromPlace, toPlace, via1, via2, via3, startTimeDay, startTimeHour, timeLimitDay, timeLimitHour, moneyLimit, priority):
        print("接收到的行程要求：", fromPlace, toPlace, via1, via2, via3, startTimeDay,
              startTimeHour, timeLimitDay, timeLimitHour, moneyLimit, priority)

        global GTime
        if startTimeDay < GTime[0] or (startTimeDay == GTime[0] and startTimeHour < GTime[1]):
            self.popMessageHandler("非法输入：所查询最早发车时间早于当前时间！")
            return Null

        viaList = []
        if via1:
            viaList.append(via1)
        if via2:
            viaList.append(via2)
        if via3:
            viaList.append(via3)

        startTime = [0, 0, 0]
        if startTimeDay:
            startTime[0] = startTimeDay
        if startTimeHour:
            startTime[1] = startTimeHour

        timeLimit = [99999, 0, 0]
        if timeLimitDay:
            timeLimit[0] = timeLimitDay
        if timeLimitHour:
            timeLimit[1] = timeLimitHour

        if not moneyLimit:
            moneyLimit = 99999

        if priority == 1:
            priority = "money"
        else:
            priority = "time"

        plan = self.Algorithm.search(
            fromPlace, toPlace, viaList, startTime, timeLimit, moneyLimit, priority)

        self.lprint("用户查询了由"+fromPlace+"发往"+toPlace+"的路线")
        return plan

    @pyqtSlot(str, result=bool)
    def deleteSchedule(self, schedule):
        schedule = eval(schedule)
        for cs in self.scheduleQueue.queue:
            if cs.schedule == schedule:
                self.scheduleQueue.queue.remove(cs)
                break
        self.lprint("用户取消了由"+schedule["from"]+"发往"+schedule["to"]+"的行程")
        return True

    @pyqtSlot(str, result=bool)
    def drawSingleSchedule(self, schedule):
        schedule = eval(schedule)
        self.Algorithm.drawMap(schedule=schedule)
        return True

    @pyqtSlot(str, result=bool)
    def drawWholePlan(self, plan):
        plan = eval(plan)
        self.Algorithm.drawMap(plan=plan)
        return True

    @pyqtSlot(str, result=bool)
    def acceptPlan(self, plan):
        plan = eval(plan)
        if not plan["scheduleList"]:
            return False
        for s in plan["scheduleList"]:
            print(str(s))
            cs = ComparableSchedule(s)
            self.scheduleQueue.put(cs)
        print(plan)
        self.popMessageHandler(
            "成功添加由"+plan["scheduleList"][0]["from"]+"发往"+plan["scheduleList"][-1]["to"]+"的列车。")
        self.lprint("用户添加了由"+plan["scheduleList"][0]
                    ["from"]+"到"+plan["scheduleList"][-1]["to"]+"的计划")
        return True

    @pyqtSlot(str, result=bool)
    def echo(self, echoMessage):
        # 测试函数
        print(echoMessage)
        return True

    def lprint(self, message):
        # 日志打印
        global GTime
        k = GTime[0:2]
        if not self.log.get(str(k)):
            self.log[str(k)] = []
        self.log[str(k)].append(message)
        json.dump(self.log, open("log.json", "w"), ensure_ascii=False)


if __name__ == '__main__':
    # 界面初始化
    app = QtWidgets.QApplication(sys.argv)
    engine = QtQml.QQmlApplicationEngine()
    context = engine.rootContext()

    # 配置qml属性
    system = System()
    context.setContextProperty("system", system)

    timer = Timer()
    context.setContextProperty("timer", timer)

    # 加载qml属性
    engine.addImportPath(absPath+"qtquickcontrols2.conf")
    engine.load(QUrl(absPath+'main.qml'))
    engine.load(QUrl(absPath+'loginPage.qml'))

    # 信号与槽连接
    rootObject = engine.rootObjects()[0]
    rootObject.stopTime.connect(timer.stopTime)
    rootObject.startTime.connect(timer.continueTime)
    rootObject.changeTimeSpeed.connect(timer.changeSpeed)
    moveTimeHandler.timeChanged.connect(moveTimeHandler.moveTime)

    sys.exit(app.exec_())
    os.system("pause")
