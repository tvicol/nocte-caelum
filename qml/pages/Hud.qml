/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/



import QtQuick 2.0
import Sailfish.Silica 1.0


Item {

    anchors.fill: parent

    property bool locationVisible: true
    property bool datetimeVisible: true
    property bool timerateVisible: true
    property bool fpsVisible: true
    property bool fovVisible: true
    property bool buttonsVisible: true
    property double transparency: 0.0
    property double infoTransparency: 0.0
    property double hudOpacity: 1.0

    Component { id: objSearch; ObjectSearch {} }


    Item {
        anchors.fill: parent
        //opacity: 1 - infoTransparency

        Label {
            id: name
            text: stelMainSkyItem.nameText
            color: stelMainSkyItem.nameColor
            textFormat: Text.StyledText
            wrapMode: Text.NoWrap
            font.pixelSize: Theme.fontSizeSmall
            font.bold: true
            anchors {
                left: parent.left
                top: parent.top
            }
            onTextChanged: {
                if (name.text == "")
                    infoText.anchors.top = parent.top
                else {
                    infoText.anchors.top = name.bottom
                    infoText.anchors.topMargin = 5
                }
            }
        }
        Label {
            id: infoText
            text: stelMainSkyItem.infoText
            color: stelMainSkyItem.infoColor
            textFormat: Text.StyledText
            wrapMode: Text.NoWrap
            width: parent.width
            font.pixelSize: Theme.fontSizeTiny
            anchors.left: parent.left
        }
    }

    Item {
        anchors.fill: parent
        //opacity: 1 - transparency

        IconButton {
            icon.source: "image://theme/icon-m-dismiss"
            visible: stelMainSkyItem.nameText != ""
            onClicked: {
                stelMainSkyItem.clearSelection()
                stelMainSkyItem.nameText = ""
                stelMainSkyItem.infoText = ""
            }
            anchors {
                top: parent.top
                right: parent.right
            }
        }

        Row {
            id: row1
            Label {
                visible: locationVisible
                text: appWin.stelMainSkyItem.location
                font.pixelSize: Theme.fontSizeSmall
                width: 255
            }

            Label {
                visible: timerateVisible
                text: "Time rate: " + appWin.stelMainSkyItem.timerate + "x"
                font.pixelSize: Theme.fontSizeSmall
            }
            anchors {
                left: parent.left
                leftMargin: 20
                bottom: row2.top
            }

        }
        Row {
            id: row2

            Label {
                visible: fovVisible
                text: stelMainSkyItem.fov.toPrecision(stelMainSkyItem.fov > 0 ? 3 : 1)+ " FOV"
                font.pixelSize: Theme.fontSizeSmall
                width: 135
            }
            Label {
                visible: fpsVisible
                text: stelMainSkyItem.fps.toFixed(0) + " FPS"
                font.pixelSize: Theme.fontSizeSmall
                width: 85
            }
            Label {
                visible: datetimeVisible
                text: stelMainSkyItem.prettyDatetime
                font.pixelSize: Theme.fontSizeSmall
            }
            anchors {
                left: parent.left
                leftMargin: 20
                bottom: buttonsVisible ? row3a.top : parent.bottom
                bottomMargin: buttonsVisible ? 0 : 10
            }
        }

        Row {

            id: row3a

            //visible: appWin.stelMainSkyItem.buttonsVisible

            IconButton {
                icon.source: "image://theme/icon-m-back"
                onClicked: stelMainSkyItem.decreaseTimeSpeed()
            }

            IconButton {
                icon.source: "image://theme/icon-m-media"
                onClicked: stelMainSkyItem.toggleRealtimeSpeed()
            }

            IconButton {
                icon.source: "image://theme/icon-m-forward"
                onClicked: stelMainSkyItem.increaseTimeSpeed()
            }

            IconButton {
                icon.source: "image://theme/icon-m-clock"
                onClicked: stelMainSkyItem.setTimeToNow()
            }

            IconButton {
                icon.source: "image://theme/icon-m-search"
                onClicked: (name.text == "") ?  pageStack.push(objSearch) : stelMainSkyItem.centerSelection()
            }

            anchors {
                left: parent.left
                bottom: parent.bottom
            }

        }

        Row {
            id: row3b
            layoutDirection: Qt.RightToLeft
            anchors {
                right: parent.right
                bottom: parent.bottom
            }

            Component { id: settingsPage ; QuickSettings {} }

            IconButton {
                icon.source: "image://theme/icon-m-developer-mode"
                onClicked: {
                    pageStack.push(settingsPage)
                }

            }
        }
    }

}
