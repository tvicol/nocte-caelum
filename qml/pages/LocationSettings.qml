/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import Sailfish.Silica 1.0


DimmedPage {

    id: page

    property string latH
    property double latD
    property double latM
    property double latS

    property string longH
    property double longD
    property double longM
    property double longS

    Component.onCompleted: {
        stelMainSkyItem.locationLatitudeChanged.connect(onLatitudeChanged)
        stelMainSkyItem.locationLongitudeChanged.connect(onLongitudeChanged)
        onLatitudeChanged(stelMainSkyItem.locationLatitude)
        onLongitudeChanged(stelMainSkyItem.locationLongitude)
    }

    Component.onDestruction: {
        stelMainSkyItem.locationLatitudeChanged.disconnect(onLatitudeChanged)
        stelMainSkyItem.locationLongitudeChanged.disconnect(onLongitudeChanged)
    }


    function onLatitudeChanged(latitude)
    {
        latH = latitude >= 0 ? "N" : "S"
        var d = Math.abs(latitude)
        var m = d * 60.
        latD = Math.floor(d)
        latM = Math.floor((d - Math.floor(d)) * 60.)
        latS = Math.round((m - Math.floor(m)) * 60.)
    }

    function onLongitudeChanged(longitude)
    {
        longH = longitude >= 0 ? "E" : "W"
        var d = Math.abs(longitude)
        var m = d * 60.
        longD = Math.floor(d)
        longM = Math.floor((d - Math.floor(d)) * 60.)
        longS = Math.round((m - Math.floor(m)) * 60.)
    }

    function updateLocation(newLoc)
    {
        if (!stelMainSkyItem.locationIsNew)
        {
            if (newLoc)
                name = "New location"
            stelMainSkyItem.locationIsNew = true
        }
        stelMainSkyItem.applyCurrentLocation()
    }

    Component { id: locSearch; LocationSearch {} }

//    PositionSource {
//        id: positionSource
//        active: false
//        //preferredPositioningMethods: PositionSource.SatellitePositioningMethods
//        onPositionChanged: {
//            console.log("Position changed, haccuracy: " + position.horizontalAccuracy)
//            console.log("Error " + sourceError)
//            if (sourceError != PositionSource.NoError)
//            {
//                console.log("exit")
//                return;
//            }

//            if (!active)
//                return;

//            if (position.altitudeValid)
//                stelMainSkyItem.locationAltitude = position.coordinate.altitude

//            if (position.longitudeValid && position.latitudeValid)
//            {
//                stelMainSkyItem.locationLongitude = position.coordinate.longitude
//                stelMainSkyItem.locationLatitude = position.coordinate.latitude
//                updateLocation(true)
//                positionRow.subtitle = "Position updated."
//                console.log("Position updated")
//                stop()
//            }
//            else
//            {
//                positionRow.subtitle = "Could not get position from device."
//                console.log("Could not get position")
//            }
//            active = false
//        }
//    }

    SilicaFlickable {

        anchors.fill: parent
        contentHeight: column.height

        VerticalScrollDecorator {}
        Column {

            x: Theme.paddingLarge
            id: column
            width: page.width - 2 * Theme.paddingLarge
            spacing: Theme.paddingLarge

            PageHeader {
                title: "Location Settings"
            }

            ListRow {
                title: "Select location from list"
                onClicked: pageStack.push(locSearch)
            }

//            ListRow {
//                id: positionRow
//                visible: false
//                title: "Update from device position"
//                onClicked:  {
//                    if (positionSource.positioningMethod == PositionSource.NoPositioningMethod)
//                    {
//                        subtitle = "No positioning method available."
//                    }
//                    else
//                    {
//                        positionSource.start()
//                        positionSource.update()
//                        subtitle = "Retrieving position..."
//                        console.log("Retreiving position")
//                    }
//                }
//            }

            TextField {
                width: parent.width
                label: "Name"
                text: stelMainSkyItem.locationName
                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: {
                    focus = false;
                    Qt.inputMethod.hide()
                    stelMainSkyItem.locationName = text
                    updateLocation(false)
                }
            }

            TextField {
                width: parent.width
                label: "Country"
                text: stelMainSkyItem.locationCountry
                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: {
                    focus = false;
                    Qt.inputMethod.hide()
                    stelMainSkyItem.locationCountry = text
                    updateLocation(false)
                }
            }

            TextField {
                width: parent.width
                label: "Planet"
                text: stelMainSkyItem.locationPlanet
                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: {
                    focus = false;
                    Qt.inputMethod.hide()
                    stelMainSkyItem.locationPlanet = text
                    updateLocation(false)
                }
            }

            TextField {
                width: parent.width
                label: "Latitude"
                text: latH + " " + latD + "d " + latM + "m " + latS + "s"
                readOnly: true
            }

            TextField {
                width: parent.width
                label: "Longitude"
                text: longH + " " + longD + "d " + longM + "m " + longS + "s"
                readOnly: true
            }

            TextField {
                width: parent.width
                label: "Altitude (meters)"
                text: stelMainSkyItem.locationAltitude
                inputMethodHints: Qt.ImhPreferNumbers
                validator: IntValidator { bottom: -200 ; top: 30000 }
                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: {
                    focus = false;
                    Qt.inputMethod.hide()
                    stelMainSkyItem.locationAltitude = text
                    updateLocation(false)
                }
            }


            Button {
                text: "Add to List"
                anchors.horizontalCenter: parent.horizontalCenter
                height: Theme.itemSizeMedium
                onClicked: {
                    stelMainSkyItem.addLocationToList()
                    stelMainSkyItem.locationIsNew = false
                    stelMainSkyItem.locationCanBeDeleted = true
                }
                visible: stelMainSkyItem.locationIsNew
            }

            Button {
                text: "Remove from List"
                anchors.horizontalCenter: parent.horizontalCenter
                height: Theme.itemSizeMedium
                onClicked:  {
                    stelMainSkyItem.removeLocationFromList()
                    stelMainSkyItem.locationIsNew = true
                    stelMainSkyItem.locationCanBeDeleted = false
                }
                visible: stelMainSkyItem.locationCanBeDeleted
            }

            Item {
                id: item
                width: parent.width
                height: parent.width
                Image {
                    id: image
                    source: stelMainSkyItem.terrainImageSource
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent

                    MouseArea {
                        anchors.centerIn: parent
                        width: image.paintedWidth
                        height: image.paintedHeight
                        onClicked: {
                            var _x = (mouse.x - (image.paintedWidth) * 0.5) / image.paintedWidth * 2.
                            var _y = (mouse.y - (image.paintedHeight) * 0.5) / image.paintedHeight * 2.

                            //console.log("iw " + image.paintedWidth + " ih " + image.paintedHeight)
                            //console.log("mx " + mouse.x + " my " + mouse.y + " long " + x * 180. + " lat " + y * -90.)
                            stelMainSkyItem.locationLongitude = _x * 180.
                            stelMainSkyItem.locationLatitude = -_y * 90.
                            updateLocation(true)
                        }
                    }
                }
            }

        }

    }

}
