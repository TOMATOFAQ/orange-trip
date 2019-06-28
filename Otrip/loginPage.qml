import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Material 2.3
import QtQuick.Layouts 1.3
import com.faq.helloworld 1.0
Page {
    id:loginPage
    visible: true
    width: 640
    height: 480

    title: qsTr("携🍊旅行")
    Material.theme: Material.Light
    Material.accent: Material.Orange

    HelloWorld{
        id:helloworld
    }

    Rectangle {
        id: logo
        width: parent.width
        height: parent.height * 0.5
        color: Material.background
        Text {
            id: logoText
            text: qsTr("🍊")
            anchors.centerIn: parent
            font.pointSize: 150
        }
    }

    ColumnLayout {
        width: parent.width
        anchors.top: logo.bottom
//        spacing: 15

        TextField {
            id: loginUsername
            placeholderText: qsTr("User name")
            Layout.preferredWidth: parent.width * 0.69
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 15

            leftPadding: 40
            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 50
                radius: implicitHeight / 2
                color: "transparent"

                Text {
                    text:"🌝"
                    font.pointSize: 15

                    color: Material.accent
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    leftPadding: 10
                }

                Rectangle {
                    width: parent.width - 10
                    height: 1
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    color: Material.accent
                }
            }
            Keys.onReturnPressed: {
                login()
            }
        }

        TextField {
            id: loginPassword
            placeholderText: qsTr("Password")
            Layout.preferredWidth: parent.width *0.7
            Layout.alignment: Qt.AlignHCenter
            font.pointSize: 15

            leftPadding: 40
            echoMode: TextField.Password
            background: Rectangle {
                implicitWidth: 200
                implicitHeight: 50
                radius: implicitHeight / 2
                color: "transparent"
                Text {
                    text: "🌚"
                    font.pointSize: 14

                    color: Material.accent
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                    leftPadding: 10
                }

                Rectangle {
                    width: parent.width - 10
                    height: 1
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottom: parent.bottom
                    color: Material.accent
                }
            }
            Keys.onReturnPressed: {
                login()
            }
        }

            RowLayout {
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                RadioButton {
                    id: gamerButton
                    text: qsTr("Player")
                    checked: true
                }
                RadioButton {
                    text: qsTr("Builder")
                }
            }

        RowLayout {
            Layout.preferredWidth: parent.width * 0.6
            Layout.preferredHeight: 69 // layout高度
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter

            Button {
                Layout.preferredWidth: parent.width * 0.4
                Layout.preferredHeight: parent.height * 0.7
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                id: loginButton
                text: qsTr("Login")
                onClicked: login()
            }

            Button {
                Layout.preferredWidth: parent.width * 0.4
                Layout.preferredHeight: parent.height * 0.7
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                id: registerButton
                text: qsTr("Register")
                onClicked: {
                    helloworld.sayHellow()
                    /*Do nothing*/
                }
            }
        }

    } // ColoumLayout

    /*逻辑部分*/

}
