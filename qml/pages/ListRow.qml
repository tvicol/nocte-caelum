/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

BackgroundItem {

    id: item
    property string title
    property string subtitle

    //width: view.width
    height: Theme.itemHeightSmall

    Item {
        anchors.fill: parent

        Column {
            anchors.verticalCenter: parent.verticalCenter
            Label {
                visible: subtitle != ""
                text: title
                font.pixelSize: Theme.fontSizeLarge
                color: item.highlighted ? Theme.highlightColor : Theme.primaryColor
            }
            Label {
                text: subtitle
                font.pixelSize: Theme.fontSizeSmall
                color: item.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
            }
        }

        Column {
            visible: subtitle == ""
            anchors.verticalCenter: parent.verticalCenter
            Label {
                text: title
                font.pixelSize: Theme.fontSizeSmall
                color: item.highlighted ? Theme.highlightColor : Theme.primaryColor
            }
        }
    }

}
