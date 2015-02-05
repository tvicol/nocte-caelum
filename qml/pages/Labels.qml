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
        title: "Label settings"
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


        Slider {
            width: parent.width
            height: Theme.itemSizeMedium
            label: "Star labels"
            minimumValue: 0
            maximumValue: 5
            value: stelMainSkyItem.starLabelsAmount
            onValueChanged: stelMainSkyItem.starLabelsAmount = value
        }

        Slider {
            width: parent.width
            height: Theme.itemSizeMedium
            label: "Planet labels"
            minimumValue: 0
            maximumValue: 5
            value: stelMainSkyItem.planetLabelsAmount
            onValueChanged: stelMainSkyItem.planetLabelsAmount = value
        }

        Slider {
            width: parent.width
            height: Theme.itemSizeMedium
            label: "Nebula labels"
            minimumValue: 0
            maximumValue: 5
            value: stelMainSkyItem.nebulaLabelsAmount
            onValueChanged: stelMainSkyItem.nebulaLabelsAmount = value
        }
    }


}
