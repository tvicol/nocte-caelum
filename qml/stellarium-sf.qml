/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/



import QtQuick 2.0
import Sailfish.Silica 1.0
import StellariumSF 1.0
import "pages"

ApplicationWindow
{
    property StelMainSkyItem stelMainSkyItem
    property string version: "0.9.1"

    id: appWin
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: Orientation.Portrait | Orientation.Landscape
    initialPage: mainPage



    Component {
        id: mainPage
        Page {

            id: page

            allowedOrientations: appWin.allowedOrientations
            onOrientationChanged: skyItem.orientation = orientation

            onStatusChanged: {
                if (status == PageStatus.Deactivating)
                    skyItem.skipNumFrames = 3
                else if (status == PageStatus.Active)
                    skyItem.skipNumFrames = 0
            }

            StelMainSkyItem {
                id: skyItem
                Component.onCompleted: {
                    stelMainSkyItem = skyItem
                }

                anchors.fill: parent
                onLoaded: {
                    splash.visible = false
                    hud.visible = true
                    appWin._backgroundVisible = false
                    orientation = parent.orientation
                }
            }

            Hud {
                id: hud
                visible: false
            }

            Item {
                id: splash
                anchors.fill: parent
                Column {
                    id: column
                    anchors.centerIn: parent
                    Label {
                        text: "Stellarium N9"
                        color: Theme.primaryColor
                        font {
                            pixelSize: 40
                            bold: true
                        }
                        anchors.horizontalCenter: column.horizontalCenter
                    }
                    Label {
                        text: version
                        color: Theme.primaryColor
                        font {
                            pixelSize: 32
                            bold: true
                        }
                        anchors.horizontalCenter: column.horizontalCenter
                    }
                    Item { height: Theme.paddingLarge ; width: parent.width }
                    Image {
                        anchors.horizontalCenter: column.horizontalCenter
                        source: "stellarium-sf.png"
                    }
                }

                Label {
                    text: "Based on Stellarium 0.12.4"
                    color: Theme.primaryColor
                    font.pixelSize: 26
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        bottom: parent.bottom
                        bottomMargin: 50
                    }
                }
            }


        }
    }


}



