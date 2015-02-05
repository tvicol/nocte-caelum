/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import Sailfish.Silica 1.0


SearchPage {

    function onLocationFound()
    {
        pageStack.pop()
    }

    Component.onCompleted: stelMainSkyItem.locationFound.connect(onLocationFound)
    Component.onDestruction: stelMainSkyItem.locationFound.disconnect(onLocationFound)

    pageHeader: "Location Search"
    searchString: stelMainSkyItem.locationSearchString
    onSearchStringChanged: stelMainSkyItem.locationSearchString = searchString
    model: stelMainSkyItem.locationSearchResultsModel

    onUpdateSearchResults: stelMainSkyItem.updateLocationSearchResults()
    onItemClicked: stelMainSkyItem.selectNamedLocation(name, recheck)

}
