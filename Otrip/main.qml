import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3

ApplicationWindow {
    visible: true
    width: 640
    height: 560
    title: qsTr("携橙旅行🍊")
    Material.theme: Material.Light
    Material.accent: Material.Orange
    signal clickQuery();
    signal updateTime();
    signal changeTimeSpeed();
    signal stopTime();
    signal startTime();
    signal submitDemand();
    // 属性当全局变量用，在各模块间传参
    property var lastPlan: "{'arrivedTime': [2, 22, 38], 'scheduleList': [{'arrivedTime': [1, 8, 58], 'date': [0, 17, 10], 'duration': [0, 15, 48], 'end': [1, 8, 58], 'from': '广州', 'number': 'Z14', 'price': 192, 'start': [0, 17, 10], 'to': '郑州', 'type': 'Z'}, {'arrivedTime': [2, 1, 59], 'date': [1, 21, 24], 'duration': [0, 4, 35], 'end': [    1, 1, 59], 'from': '郑州', 'number': 'K118', 'price': 62, 'start': [0, 21, 24], 'to': '石家庄', 'type': 'K'}, {'arrivedTime': [2, 22, 38], 'date': [2, 18, 35], 'duration': [0, 4, 3], 'end': [0, 22, 38], 'from': '石家庄', 'number': 'K5216', 'price': 24, 'start': [0, 18, 35], 'to': '北京', 'type': 'K'}],'totalPrice': 278}"
    

    StackView{
        id: stackView
        focus: true
        anchors.fill: parent
    }

    Component.onCompleted: {
        stackView.push("qrc:ui/loginPage.qml")
    }

    
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex

        Page1Form {
                width: parent.width/2
                height: 560
                Button {
                    anchors.top:parent.top
                    id:myTime
                    text: 'Default'
                        onClicked:{
                    changeTimeSpeed()
                    }
                }

                Text{
                    x:parent.width/2
                    text:"当前行程安排："
                    font.pixelSize: 20
                }

                Column {
                    id: startColumn
                    x: 10
                    y: parent.height/6
                    width: parent.width/2
                    height: parent.height/2

                    Text{
                        text:"身处："
                        font.pixelSize: 20
                    }

                    Text{
                        text:"由："
                        font.pixelSize: 20
                    }


                    Text{
                        text:"发往："
                        font.pixelSize: 20
                    }


                    Text{
                        text:"预计抵达时间："
                        font.pixelSize: 20
                    }
                }

                Column {
                    x: 10
                    y: parent.height/6
                    width: parent.width/2
                    height: parent.height/2

                    Text{
                        id:statusLocation
                        x:parent.width/2
                        text:""
                        font.pixelSize: 20
                    }

                    Text{
                        id:statusStart
                        x:parent.width/2
                        text:""
                        font.pixelSize: 20
                    }


                    Text{
                        id:statusTo
                        x:parent.width/2
                        text:""
                        font.pixelSize: 20
                    }


                    Text{
                        id:statusArrivedTime
                        x:parent.width/2
                        text:"鸿蒙"
                        font.pixelSize: 20
                    }

                }

                ListView {
                    id: scheduleQueue
                    x: parent.width / 2
                    y: 100
                    width: parent.width / 2
                    height: parent.height
                    model: ListModel {

                    }

                    delegate: Item {
                        x: 5
                        width: 80
                        height: 100
                        Row {
                            Column{                            
                                Button {
                                    width: 60
                                    height: 40
                                    text:qsTr("详情")
                                    onClicked:{
                                        system.drawSingleSchedule(scheduleAsString)
                                    }
                                }

                                Button {
                                    width: 60
                                    height: 40
                                    text:qsTr("删除")
                                    onClicked:{
                                        system.deleteSchedule(scheduleAsString)
                                        updateScheduleQueue()
                                    }
                                }
                            }

                            Column{
                                Text {
                                    text:"来自："+from
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.bold: true
                                }

                                Text {
                                    text:"前往："+to
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.bold: true
                                }

                                Text {
                                    text:"出发时间："+date
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.bold: true
                                }


                                Text {
                                    text:"抵达时间："+arrivedTime
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.bold: true
                                }

                                Text {
                                    text:"车号:"+ number
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.bold: true
                                }
                                
                            }
                    
                            spacing: 10
                        }
                    }
                }
        }

        Page2Form {
            id: page
            width: 600
            height: 400

                // 写在前面 - 逻辑部分
                Button {
                    id: submitDemandButton
                    x: 17
                    y: 383
                    width: 212
                    height: 48
                    text: qsTr("行程查询")
                    onClicked: {
                        var plan = system.receiveDemand(fromPlace.currentText, toPlace.currentText, via1.currentText, via2.currentText, via3.currentText, startTimeDay.text, startTimeHour.text, timeLimitDay.text, timeLimitHour.text, moneyLimit.text, moneyPriority.checked)
                        for (var key in plan){
                            console.log(key + ":" + plan[key])
                        }
                        lastPlan = JSON.stringify(plan)

                        totalPriceText.text = plan["totalPrice"]
                        arrivedTimeText.text = String(plan["arrivedTime"])

                        resultScheduleList.model.clear()
                        for(var schedule of plan["scheduleList"]){ // javascript 他妈的个王八蛋，你个 for 循环干什么返回下标？？？浪费两个小时看你一个bug？？？ for-in 和 for-of 还两种不同的语法？？
                            var scheduleAsString = JSON.stringify(schedule)
                            console.log(scheduleAsString)
                            
                            resultScheduleList.model.append({
                                "from": schedule["from"],
                                "to":schedule["to"],
                                "date":String(schedule["date"]),
                                "arrivedTime":String(schedule["arrivedTime"]),
                                "number":String(schedule["number"]),
                                "price":String(schedule["price"]),
                                "type":schedule["type"],
                                "scheduleAsString":scheduleAsString
                            })
                        }
                    }
                }

                Button {
                    id: drawPlanButton
                    x: 550
                    y: 10
                    text: qsTr("地图")
                    font.family: "fontawesome"
                    onClicked:{
                        system.drawWholePlan(lastPlan)
                    }
                }

                Button {
                    id: acceptPlanButton
                    x: 550
                    y: 60
                    text: qsTr("接受行程")
                    onClicked:{
                        system.acceptPlan(lastPlan)
                    }
                }

                ListView {
                    id: resultScheduleList
                    x: parent.width / 2
                    y: parent.height / 3
                    width: parent.width / 2
                    height: parent.height / 2
                    model: ListModel {
                        ListElement {
                            from: "半人马α座"
                            to:"地球"
                            date:"[0,0,0]"
                            arrivedTime:"[1461,0,0]"
                            number:"第一舰队"
                            price:"MAX"
                            type:"N"
                            scheduleAsString:"{'arrivedTime': [2, 22, 38], 'date': [2, 18, 35], 'duration': [0, 4, 3], 'end': [0, 22, 38], 'from': '广州', 'number': 'K5216', 'price': 24, 'start': [0, 18, 35], 'to': '北京', 'type': 'K'}"
                        }
                        ListElement {
                            from: "半人马α座"
                            to:"地球"
                            date:"[0,0,0]"
                            arrivedTime:"[400,0,0]"
                            number:"第二舰队"
                            price:"MAX"
                            type:"G"
                            scheduleAsString:"{'arrivedTime': [2, 22, 38], 'date': [2, 18, 35], 'duration': [0, 4, 3], 'end': [0, 22, 38], 'from': '广州', 'number': 'K5216', 'price': 24, 'start': [0, 18, 35], 'to': '北京', 'type': 'K'}"
                        }
                    }

                    delegate: Item {
                        x: 5
                        width: 80
                        height: 100
                        Row {
                            id: row1
                            Button {
                                width: 60
                                height: 40
                                text:qsTr("详情")
                                onClicked:{
                                    system.drawSingleSchedule(scheduleAsString)
                                }
                            }

                            Column{
                                Text {
                                    text:"来自："+from
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.bold: true
                                }

                                Text {
                                    text:"前往："+to
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.bold: true
                                }

                                Text {
                                    text:"出发时间："+date
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.bold: true
                                }


                                Text {
                                    text:"抵达时间："+arrivedTime
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.bold: true
                                }

                                Text {
                                    text:"车号:"+ number
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    font.bold: true
                                }
                                
                            }
                    
                            spacing: 10
                        }
                    }
                }

                Label {
                    id: label
                    x: 294
                    y: 19
                    width: 133
                    height: 36
                    text: qsTr("总金钱消耗:")
                    font.pointSize: 20
                }

                Text {
                    id: totalPriceText
                    x: 412
                    y: 19
                    width: 109
                    height: 36
                    text: qsTr("四颗恒星")
                    renderType: Text.NativeRendering
                    font.pixelSize: 20
                }

                Label {
                    id: label1
                    x: 294
                    y: 61
                    width: 133
                    height: 36
                    text: qsTr("抵达时间：")
                    font.pointSize: 20
                }

                Text {
                    id: arrivedTimeText
                    x: 412
                    y: 61
                    width: 112
                    height: 28
                    text: qsTr("三百年")
                    font.pixelSize: 20
                }

                // 城市下拉列表
                Column {
                    id: columnCityChoice
                    x: 18
                    y: 20
                    width: 216
                    height: 205

                    RowLayout {
                        id: rowLayoutFrom
                        width: parent.width
                        height: parent.height / 5
                        transformOrigin: Item.Center
                        anchors.top: parent.top

                        spacing: 5
                        Text {
                            id: fromText
                            x: 29
                            y: 29
                            text: qsTr("来自:")
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            font.pixelSize: 20
                        }

                        ComboBox {
                            id: fromPlace
                            y: 19
                            width: parent.width / 2
                            height: parent.height
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            // anchors.left: Layout.alignment
                            // anchors.bottom: startTime.bottom

                            wheelEnabled: false
                            focusPolicy: Qt.NoFocus
                            enabled: true
                            editable: false
                            flat: false
                            // displayText: ""
                            model: ['', '上海', '乌鲁木齐', '兰州', '北京', '南京', '南宁', '合肥', '呼和浩特', '哈尔滨', '天津', '太原', '广州', '成都', '拉萨', '昆明', '杭州', '武汉', '济南', '海口', '石家庄', '福州', '西宁', '西安', '贵阳', '郑州', '重庆', '银川', '长春', '长沙']
                        }
                    }

                    RowLayout {
                        id: rowLayoutTo
                        width: parent.width
                        height: parent.height / 5
                        transformOrigin: Item.Center

                        anchors.top: rowLayoutFrom.bottom

                        spacing: 5

                        Text {
                            id: toText
                            x: 29
                            y: 76
                            text: qsTr("前往:")
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            font.pixelSize: 20
                        }

                        ComboBox {
                            id: toPlace
                            x: 126
                            y: 66
                            width: parent.width / 2
                            height: parent.height
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            model: ['', '上海', '乌鲁木齐', '兰州', '北京', '南京', '南宁', '合肥', '呼和浩特', '哈尔滨', '天津', '太原', '广州', '成都', '拉萨', '昆明', '杭州', '武汉', '济南', '海口', '石家庄', '福州', '西宁', '西安', '贵阳', '郑州', '重庆', '银川', '长春', '长沙']
                            enabled: true
                            wheelEnabled: false
                            flat: false
                            // anchors.left: toText.right
                            // displayText: ""
                            editable: false
                            focusPolicy: Qt.NoFocus
                        }
                    }

                    RowLayout {
                        id: rowLayoutVia1
                        width: parent.width
                        height: parent.height / 5
                        transformOrigin: Item.Center
                        anchors.top: rowLayoutTo.bottom

                        spacing: 5

                        Text {
                            id: via1Text
                            width: parent.width / 2
                            text: qsTr("途径 1:")
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            lineHeight: 1.1
                            fontSizeMode: Text.FixedSize
                            font.pixelSize: 20
                        }

                        ComboBox {
                            id: via1
                            width: parent.width / 2
                            height: parent.height
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            model: ['', '上海', '乌鲁木齐', '兰州', '北京', '南京', '南宁', '合肥', '呼和浩特', '哈尔滨', '天津', '太原', '广州', '成都', '拉萨', '昆明', '杭州', '武汉', '济南', '海口', '石家庄', '福州', '西宁', '西安', '贵阳', '郑州', '重庆', '银川', '长春', '长沙']
                            wheelEnabled: false
                            focusPolicy: Qt.NoFocus

                            enabled: true
                            // anchors.left: via1Text.right
                            editable: false
                            flat: false
                            // displayText: ""
                        }
                    }

                    RowLayout {
                        id: rowLayoutVia2
                        width: parent.width
                        height: parent.height / 5
                        anchors.top: rowLayoutVia1.bottom
                        anchors.topMargin: 0
                        Text {
                            width: parent.width / 2
                            id: via2Text
                            text: qsTr("途径 2:")
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            font.pixelSize: 20
                        }
                        ComboBox {
                            id: via2
                            width: parent.width / 2
                            height: parent.height
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            model: ['', '上海', '乌鲁木齐', '兰州', '北京', '南京', '南宁', '合肥', '呼和浩特', '哈尔滨', '天津', '太原', '广州', '成都', '拉萨', '昆明', '杭州', '武汉', '济南', '海口', '石家庄', '福州', '西宁', '西安', '贵阳', '郑州', '重庆', '银川', '长春', '长沙']
                            wheelEnabled: false
                            focusPolicy: Qt.NoFocus

                            enabled: true
                            anchors.left: via2Text.right
                            flat: false
                            editable: false
                            // displayText: ""
                        }
                    }

                    RowLayout {
                        id: rowLayoutVia3
                        width: parent.width
                        height: parent.height / 5
                        anchors.top: rowLayoutVia2.bottom
                        anchors.topMargin: 0

                        Text {
                            id: via3Text
                            text: qsTr("途径 3:")
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            font.pixelSize: 20
                            width: parent.width / 2
                        }
                        ComboBox {
                            id: via3
                            width: parent.width
                            height: parent.height / 2
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            model: ['', '上海', '乌鲁木齐', '兰州', '北京', '南京', '南宁', '合肥', '呼和浩特', '哈尔滨', '天津', '太原', '广州', '成都', '拉萨', '昆明', '杭州', '武汉', '济南', '海口', '石家庄', '福州', '西宁', '西安', '贵阳', '郑州', '重庆', '银川', '长春', '长沙']
                            wheelEnabled: false
                            focusPolicy: Qt.NoFocus

                            enabled: true
                            anchors.left: via3Text.right
                            editable: false
                            flat: false
                            // displayText: ""
                        }
                    }
                }

                RowLayout {
                    id: priority
                    x: 18
                    y: 342
                    width: 216
                    height: 35
                    anchors.right: columnVia.right
                    anchors.rightMargin: 0
                    anchors.top: columnVia.bottom
                    anchors.topMargin: 0

                    RadioButton {
                        id: moneyPriority
                        text: qsTr("金钱优先")
                        checked: true
                    }

                    RadioButton {
                        id: radioButton1
                        text: qsTr("时间优先")
                    }
                }

                Text {
                    id: startTime
                    x: 18
                    y: 237
                    width: 74
                    height: 28
                    text: qsTr("出发时间")
                    font.pixelSize: 20
                }

                Text {
                    id: text1
                    x: 17
                    y: 271
                    text: qsTr("时间限制")
                    font.pixelSize: 20
                }

                Text {
                    id: text2
                    x: 18
                    y: 308
                    text: qsTr("金钱限制")
                    font.pixelSize: 20
                }

                TextField {
                    id: startTimeDay
                    x: 109
                    y: 231
                    width: 66
                    height: 43
                    placeholderText: qsTr("日")
                    text: qsTr("")
                    font.pointSize: 13
                }

                TextField {
                    id: startTimeHour
                    x: 181
                    y: 231
                    width: 66
                    height: 43
                    placeholderText: qsTr("时")
                    text: qsTr("")
                    font.pointSize: 13
                }

                TextField {
                    id: timeLimitDay
                    x: 109
                    y: 264
                    width: 66
                    height: 43
                    placeholderText: qsTr("天")
                    text: qsTr("")
                    font.pointSize: 13
                }

                TextField {
                    id: timeLimitHour
                    x: 181
                    y: 264
                    width: 66
                    height: 43
                    placeholderText: qsTr("时")
                    text: qsTr("")
                    font.pointSize: 13
                }

                TextField {
                    id: moneyLimit
                    x: 109
                    y: 301
                    width: 138
                    height: 43
                    placeholderText: qsTr("元")
                    text: qsTr("")
                    font.pointSize: 13
                }
            }
        }
    
    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("行程管理")
            onClicked: {
                startTime();
            }
        }

        TabButton {
            text: qsTr("行程查询")
            onClicked: {
                stopTime();
            }
        }
    }

    // 同步操作组
    function synchroOperation(GTime) {
            // 其一、同步更改时间
            var GtimeList = eval(GTime)
            myTime.text = "⏩当前时间:" + GtimeList[0] + "日" + GtimeList[1] + "时" + GtimeList[2] + "分";


            //其二、同步更改行程\旅客状态
            updateScheduleQueue();
            updateStatus();

            stackView.push("qrc:loginPage.qml")
    }

    function updateStatus(){
        var status = system.updateStatus()
        statusLocation.text = status["location"]
        statusStart.text = status["start"]
        statusTo.text = status["to"]
        statusArrivedTime.text =  status["arrivedTime"][0] + "日" + status["arrivedTime"][1] + "时" + status["arrivedTime"][2] + "分"
    }


    // 异步操作
    function updateScheduleQueue(){
            var tempScheduleQueue = system.updateScheduleQueue() // 因为scheduleQueue已经作为 ID名字了
            scheduleQueue.model.clear()
            for(var schedule of tempScheduleQueue){
                var scheduleAsString = JSON.stringify(schedule)
                scheduleQueue.model.append({
                    "from": schedule["from"],
                    "to":schedule["to"],
                    "date":String(schedule["date"]),
                    "arrivedTime":String(schedule["arrivedTime"]),
                    "number":String(schedule["number"]),
                    "price":String(schedule["price"]),
                    "type":schedule["type"],
                    "scheduleAsString":scheduleAsString
                })
        }
    }

        Popup {
            id: popup
            property alias popMessage: message.text
            width : parent.width

            background: Rectangle {
                implicitWidth: parent.width
                implicitHeight: 100
                color: Material.color(Material.Orange)
            }

            y: 0
            modal: true
            focus: true
            closePolicy: Popup.CloseOnPressOutside

            Text {
                id: message
                anchors.centerIn: parent
                font.pointSize: 20
                color: "#ffffff"
            }

            onOpened: popupClose.start()
        }

    function popMessage(message){
        popup.popMessage = String(message)
        popup.open()
    }
}
