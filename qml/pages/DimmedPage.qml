/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

Page {
    property real backgroundOpacity: 0.3

    allowedOrientations: appWin.allowedOrientations
    Rectangle {
        anchors.fill: parent
        opacity: backgroundOpacity
        color: "black"
    }
}
