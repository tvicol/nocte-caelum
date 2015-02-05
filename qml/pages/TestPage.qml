/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import StellariumSF 1.0
import Sailfish.Silica 1.0

Page {
Item {

    width: 320
    height: 480


    Rectangle {
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        anchors.fill: label
        anchors.margins: -10
    }

    Text {
        id: label
        color: "black"
        wrapMode: Text.WordWrap
        text: "The background here is a squircle rendered with raw OpenGL using the 'beforeRender()' signal in QQuickWindow. This text label and its border is rendered using QML"
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 20
    }
}

}

