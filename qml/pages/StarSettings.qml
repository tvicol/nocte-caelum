/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {

    allowedOrientations: appWin.allowedOrientations

    PageHeader {
        title: "Star settings"
    }

    Rectangle {
        anchors.fill: column
        radius: 20
        color: "black"
        opacity: 0.25
        anchors.margins: -10
    }

    Column {
        id: column

        anchors {
            bottom: parent.bottom
            right: parent.right
            left: parent.left
            margins: 30
        }

        TextSwitch {
            text: "Dynamic Eye Adaptation"
            checked: stelMainSkyItem.dynamicEyeAdaptation
            onCheckedChanged: stelMainSkyItem.dynamicEyeAdaptation = checked
        }

        Slider {
            width: parent.width
            height: Theme.itemSizeMedium
            label: "Absolute Scale"
            minimumValue: 0.1
            maximumValue: 2.0
            stepSize: 0.1
            value: stelMainSkyItem.starAbsScale
            valueText: stelMainSkyItem.starAbsScale.toFixed(1)
            onValueChanged: stelMainSkyItem.starAbsScale = value
        }

        Slider {
            width: parent.width
            height: Theme.itemSizeMedium
            label: "Relative Scale"
            minimumValue: 0.2
            maximumValue: 2.0
            stepSize: 0.1
            value: stelMainSkyItem.starRelScale
            valueText: stelMainSkyItem.starRelScale.toFixed(1)
            onValueChanged: stelMainSkyItem.starRelScale = value
        }

        Slider {
            width: parent.width
            height: Theme.itemSizeMedium
            label: "Twinkle"
            minimumValue: 0
            maximumValue: 1.0
            stepSize: 0.1
            value: stelMainSkyItem.twinkleAmount
            valueText: stelMainSkyItem.twinkleAmount.toFixed(1)
            onValueChanged: stelMainSkyItem.twinkleAmount = value
        }

        ComboBox {
            width: 480
            label: "Shooting stars (ZHR)"
            anchors.horizontalCenter: parent.horizontalCenter
            Component.onCompleted: {
                var z = stelMainSkyItem.meteorZHR;
                if (z == 0)
                    currentIndex = 0
                else if (z == 80)
                    currentIndex = 2
                else if (z == 1000)
                    currentIndex = 3
                else if (z == 10000)
                    currentIndex = 4
                else if (z == 144000)
                    currentIndex = 5
                else
                {
                    stelMainSkyItem.meteorZHR = 10
                    currentIndex = 1
                }
            }

            menu: ContextMenu {
                MenuItem { text: "No shooting stars (0)" ; onClicked: stelMainSkyItem.meteorZHR = 0 }
                MenuItem { text: "Normal rate (10)" ; onClicked: stelMainSkyItem.meteorZHR = 10 }
                MenuItem { text: "Standard Perseids rate (80)" ; onClicked: stelMainSkyItem.meteorZHR = 80 }
                MenuItem { text: "Meteor storm rate (1000)" ; onClicked: stelMainSkyItem.meteorZHR = 1000 }
                MenuItem { text: "Exceptional Leonid rate (10000)" ; onClicked: stelMainSkyItem.meteorZHR = 10000 }
                MenuItem { text: "Highest rate ever (144000)" ; onClicked: stelMainSkyItem.meteorZHR = 144000 }
            }
        }
    }


}

