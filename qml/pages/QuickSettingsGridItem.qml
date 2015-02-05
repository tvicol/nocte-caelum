/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import Sailfish.Silica 1.0


BackgroundItem {
    property alias text: label.text
    property alias imageSource: image.source
    property alias image: image
    property bool active: false

    id: item

    width: gridView.cellHeight
    height: gridView.cellWidth


    Rectangle {
        visible: active
        anchors.fill: parent
        color: Theme.secondaryHighlightColor
        opacity: 0.5
    }

    Column {
        anchors.centerIn: parent

        Image {
            id: image
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Label {
            id: label
            text: "Not implemented"
            wrapMode: Text.Wrap
            font.pixelSize: Theme.fontSizeExtraSmall
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: item.width - 4
        }
    }
}
