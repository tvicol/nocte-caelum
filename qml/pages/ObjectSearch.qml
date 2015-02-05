/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

SearchPage {

    function onGotoCompleted(objectFound) {
        if (objectFound)
            pageStack.pop(stelMainSkyItem.parent)
    }

    Component.onCompleted: stelMainSkyItem.gotoCompleted.connect(onGotoCompleted)
    Component.onDestruction: stelMainSkyItem.gotoCompleted.disconnect(onGotoCompleted)

    pageHeader: "Object Search"
    searchString: stelMainSkyItem.objectSearchString
    onSearchStringChanged: stelMainSkyItem.objectSearchString = searchString
    model: stelMainSkyItem.objectSearchResultsModel

    onUpdateSearchResults: stelMainSkyItem.updateObjectSearchResults()
    onItemClicked: stelMainSkyItem.gotoNamedObject(name, recheck)

}

/*
DimmedPage {

    function onGotoCompleted(objectFound) {
        if (objectFound)
            pageStack.pop(stelMainSkyItem.parent)
    }

    Component.onCompleted: stelMainSkyItem.gotoCompleted.connect(onGotoCompleted)
    Component.onDestruction: stelMainSkyItem.gotoCompleted.disconnect(onGotoCompleted)


    Column {
        id: column
        PageHeader { title: "Search" }
        SearchField {
            id: textField
            width: view.width
            height: Theme.itemSizeMedium
            text: stelMainSkyItem.objectSearchString
            placeholderText: "Search"
            onTextChanged: {
                if (stelMainSkyItem.objectSearchString != text)
                {
                    stelMainSkyItem.objectSearchString = text
                    timer.restart()
                }
            }
            //onAccepted: stelMainSkyItem.gotoNamedObject(text, true)
        }
        z: 1
    }

    SilicaListView {

        id: view
        anchors.fill: parent
        header: Item { height: column.implicitHeight }
        currentIndex: -1
        model: stelMainSkyItem.objectSearchResultsModel

        onDragStarted: focus = true
        Timer {
            id: timer
            repeat: false
            onTriggered: stelMainSkyItem.updateObjectSearchResults()
            interval: 300
        }

        delegate: BackgroundItem {
            width: ListView.view.width
            height: Theme.itemSizeSmall
            Label {
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                text: display
            }
            onClicked: stelMainSkyItem.gotoNamedObject(display, false)
        }
    }
}

*/
