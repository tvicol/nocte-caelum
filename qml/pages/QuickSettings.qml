/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import Sailfish.Silica 1.0


DimmedPage {

    VisualItemModel {
        id: model

        QuickSettingsGridItem {
            text: "Constel. lines"
            image { source: "../../gui/btConstellationLines-on.png" }
            active: stelMainSkyItem.constLines
            onClicked: stelMainSkyItem.constLines = !stelMainSkyItem.constLines
        }

        QuickSettingsGridItem {
            text: "Constel. labels"
            image { source: "../../gui/btConstellationLabels-on.png" }
            active: stelMainSkyItem.constLabels
            onClicked: stelMainSkyItem.constLabels = !stelMainSkyItem.constLabels
        }

        QuickSettingsGridItem {
            text: "Constel. art"
            image { source: "../../gui/btConstellationArt-on.png" }
            active: stelMainSkyItem.constArt
            onClicked: stelMainSkyItem.constArt = !stelMainSkyItem.constArt
        }


//        QuickSettingsGridItem {
//            text: "Constel. boundaries"
//            image { source: "../../gui/btConstellationLines-on.png" }
//            active: stelMainSkyItem.constBoundaries
//            onClicked: stelMainSkyItem.constBoundaries = !stelMainSkyItem.constBoundaries
//        }

        QuickSettingsGridItem {
            text: "Equatorial grid"
            image { source: "../../gui/btEquatorialGrid-on.png" }
            active: stelMainSkyItem.equatorialGrid
            onClicked: stelMainSkyItem.equatorialGrid = !stelMainSkyItem.equatorialGrid
        }

        QuickSettingsGridItem {
            text: "Azimuthal grid"
            image { source: "../../gui/btAzimuthalGrid-on.png" }
            active: stelMainSkyItem.azimuthalGrid
            onClicked: stelMainSkyItem.azimuthalGrid = !stelMainSkyItem.azimuthalGrid
        }

        QuickSettingsGridItem {
            text: "Meridian line"
            image { source: "../../gui/btConstellationLines-on.png" }
            active: stelMainSkyItem.meridianLine
            onClicked: stelMainSkyItem.meridianLine = !stelMainSkyItem.meridianLine
        }

        QuickSettingsGridItem {
            text: "Ground"
            image { source: "../../gui/btGround-on.png" }
            active: stelMainSkyItem.ground
            onClicked: stelMainSkyItem.ground = !stelMainSkyItem.ground
        }

        QuickSettingsGridItem {
            text: "Atmosphere"
            imageSource: "../../gui/btAtmosphere-on.png"
            active: stelMainSkyItem.atmosphere
            onClicked: stelMainSkyItem.atmosphere = !stelMainSkyItem.atmosphere
        }

        QuickSettingsGridItem {
            text: "Cardinal points"
            image { source: "../../gui/btCardinalPoints-on.png" }
            active: stelMainSkyItem.cardinalPoints
            onClicked: stelMainSkyItem.cardinalPoints = !stelMainSkyItem.cardinalPoints
        }

        QuickSettingsGridItem {
            text: "Nebulas"
            image { source: "../../gui/btNebula-on.png" }
            active: stelMainSkyItem.nebulaLabels
            onClicked: stelMainSkyItem.nebulaLabels = !stelMainSkyItem.nebulaLabels
        }

        QuickSettingsGridItem {
            text: "Planets"
            image { source: "../../gui/btPlanets-on.png" }
            active: stelMainSkyItem.planetLabels
            onClicked: stelMainSkyItem.planetLabels = !stelMainSkyItem.planetLabels
        }

        QuickSettingsGridItem {
            text: "Labels"
            image { source: "../../gui/5-on-labels.png"; width: 32 ; height: 32 }
            active: false
            Component { id: labels ; Labels { } }
            onClicked: pageStack.push(labels)
        }

        QuickSettingsGridItem {
            text: "Search"
            image { source: "../../gui/5-on-labels.png"; width: 32 ; height: 32 }
            active: false
            Component { id: objSearch ; ObjectSearch {} }
            onClicked: pageStack.push(objSearch)
        }

        QuickSettingsGridItem {
            text: "Night mode"
            image { source: "../../gui/btNightView-on.png" }
            active: stelMainSkyItem.nightMode
            onClicked: stelMainSkyItem.nightMode = !stelMainSkyItem.nightMode
            // TODO: change qml theme
        }

        QuickSettingsGridItem {
            text: "Free look"
            image { source: "../../gui/tabicon-sky.png"; width: 32 ; height: 32 }
            active: stelMainSkyItem.freeLook
            onClicked: stelMainSkyItem.freeLook = !stelMainSkyItem.freeLook
        }

        QuickSettingsGridItem {
            text: "Atmosphere settings"
            image { source: "../../gui/btAtmosphere-on.png" }
            active: false
            Component { id: atmoSettings ; AtmosphereSettings { } }
            onClicked: pageStack.push(atmoSettings)
        }

        QuickSettingsGridItem {
            text: "Star settings"
            image { source: "../../gui/btConstellationLines-on.png" }
            active: false
            Component { id: starSettings ; StarSettings { } }
            onClicked: pageStack.push(starSettings)
        }


        QuickSettingsGridItem {
            text: "Location settings"
            image { source: "../../gui/2-on-location.png"; width: 32 ; height: 32 }
            active: false
            Component { id: locationSettings ; LocationSettings { } }
            onClicked: pageStack.push(locationSettings)
        }

        QuickSettingsGridItem {
            text: "Save settings"
            active: false
            onClicked: remorse.execute(saveCancel, "Saving settings", function() { stelMainSkyItem.saveSettings() })
            RemorseItem { id: remorse }
        }

        QuickSettingsGridItem {
            text: "About"
            active: false
            Component { id: aboutPage ; AboutPage { } }
            onClicked: pageStack.push(aboutPage)
        }
    }


    SilicaGridView {
        id: gridView
        model: model
        anchors.fill: parent

        header: PageHeader {
            title: "Settings"
        }
        cellHeight: Theme.itemSizeExtraLarge
        cellWidth: Theme.itemSizeExtraLarge

    }

    Item {
        id: saveCancel
        height: Theme.itemSizeMedium
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
    }
}

