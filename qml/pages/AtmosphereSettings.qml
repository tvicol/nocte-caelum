/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import Sailfish.Silica 1.0
import QtQuick 2.0

Page {

    allowedOrientations: appWin.allowedOrientations

    PageHeader {
        title: "Atmosphere settings"
    }

    Rectangle {
        anchors.fill: column
        radius: 20
        color: "black"
        opacity: 0.4
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
            text: "Show atmosphere"
            checked: stelMainSkyItem.atmosphere
            onCheckedChanged: stelMainSkyItem.atmosphere = checked
        }

        Slider {
            width: parent.width
            height: Theme.itemSizeMedium
            label: "Light pollution"
            minimumValue: 1
            maximumValue: 9
            stepSize: 1
            value: stelMainSkyItem.lightPollution
            valueText: stelMainSkyItem.lightPollution
            onValueChanged: stelMainSkyItem.lightPollution = value
        }


        Slider {
            width: parent.width
            height: Theme.itemSizeMedium
            label: "Pressure (mbar)"
            minimumValue: 900
            maximumValue: 1100
            stepSize: 1
            value: stelMainSkyItem.atmospherePressure
            valueText: stelMainSkyItem.atmospherePressure.toFixed(0)
            onValueChanged: stelMainSkyItem.atmospherePressure = value
        }

        Slider {
            width: parent.width
            height: Theme.itemSizeMedium
            label: "Temperature (C)"
            minimumValue: -60
            maximumValue: 60
            stepSize: 1
            value: stelMainSkyItem.atmosphereTemp
            valueText: stelMainSkyItem.atmosphereTemp.toFixed(0)
            onValueChanged: stelMainSkyItem.atmosphereTemp = value
        }

        Slider {
            width: parent.width
            height: Theme.itemSizeMedium
            label: "Extinction"
            minimumValue: 0
            maximumValue: 0.50
            stepSize: 0.01
            value: stelMainSkyItem.extinction
            valueText: stelMainSkyItem.extinction.toPrecision(2)
            onValueChanged: stelMainSkyItem.extinction = value
        }

    }


}

