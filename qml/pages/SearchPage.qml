/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import Sailfish.Silica 1.0


DimmedPage {

    property string pageHeader
    property string searchString
    property alias model: view.model
    property alias updateResultsTimer: timer

    signal itemClicked(string name, bool recheck)
    signal updateSearchResults

    Column {
        id: column
        width: parent.width
        PageHeader { title: pageHeader }
        Rectangle {
            width: parent.width - Theme.paddingSmall
            anchors.horizontalCenter: parent.horizontalCenter
            height: Theme.itemSizeMedium
            radius: 5
            color: "black"
            opacity: 0.4

            SearchField {
                id: searchField
                anchors.fill: parent
                text: searchString
                placeholderText: "Search"
                onTextChanged: {
                    if (searchString != text)
                    {
                        searchString = text
                        timer.restart()
                    }
                }
            }
        }
        z: 1
    }

    SilicaListView {

        id: view
        anchors {
            fill: parent
            leftMargin: Theme.paddingLarge
            rightMargin: Theme.paddingLarge
        }
        header: Item { height: column.implicitHeight }
        currentIndex: -1

        onDragStarted: focus = true
        Timer {
            id: timer
            repeat: false
            onTriggered: updateSearchResults()
            interval: 300
        }

        delegate: BackgroundItem {
            width: ListView.view.width
            height: Theme.itemSizeSmall
            Label {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 20
                text: display
            }
            onClicked: itemClicked(display, false)
        }
    }
}

