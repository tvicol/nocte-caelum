/*
  Stellarium for Sailfish and Stellarium N9 Copyright (C) 2012-2014 Marko Srebre
  Contact: Marko Srebre <starstuffn9@gmail.com>
  GNU GPL 2
*/

import QtQuick 2.0
import Sailfish.Silica 1.0

import QtWebKit 3.0

Page {

    DimmedBackground { opacity: 0.6 }

    id: page

    SilicaFlickable {

        anchors.fill: parent
        contentHeight: column.height

        VerticalScrollDecorator {}

        Column {

            id: column
            width: page.width - Theme.paddingLarge * 2
            x: Theme.paddingLarge

            PageHeader {
                title: "About"
            }

            Label {
                text: "Stellarium for Sailfish"
                font.pixelSize: Theme.fontSizeExtraLarge
                font.bold: true
            }
            Label {
                text: "Version " + appWin.version
                font.pixelSize: Theme.fontSizeSmall
            }

            Item {  height: Theme.paddingLarge * 1.5; width: column.width }
            Item {  height: Theme.paddingSmall ; width: column.width }

            Label {
                text: 'Web: <a href=\"https://github.com/skorjevec/stel-sf\">https://github.com/skorjevec/stel-sf</a>'
                onLinkActivated: { Qt.openUrlExternally(link) }
                font.pixelSize: Theme.fontSizeSmall
            }

            Label {
                text: "Mail: starstuffn9@gmail.com"
                font.pixelSize: Theme.fontSizeSmall
            }

            Item {  height: Theme.paddingLarge * 2 ; width: column.width }

            Label {
                text: "If you wish to support the development of Stellarium for Sailfish, please use the following links. Thank you."
                wrapMode: Text.WordWrap
                width: parent.width
                font.pixelSize: Theme.fontSizeSmall
            }

            Item {  height: Theme.paddingSmall ; width: column.width }

            BackgroundItem
            {
                height: Theme.itemSizeLarge
                width: parent.width
                Column {
                    spacing: Theme.paddingSmall
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter

                    Label {
                        text: "Paypal or Credit Card"
                        font.pixelSize: Theme.fontSizeSmall
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                    Image {
                        id: image
                        anchors.horizontalCenter: parent.horizontalCenter
                        source: "paypal.gif"
                    }
                }
                onClicked: Qt.openUrlExternally("https://www.paypal.com/cgi-bin/webscr?cmd=_donations&business=AWF6SVUUXZBYN&lc=SI&item_name=Stellarium%20for%20Sailfish&currency_code=EUR&bn=PP%2dDonationsBF%3abtn_donateCC_LG%2egif%3aNonHosted")
            }

            BackgroundItem
            {
                height: Theme.itemSizeLarge
                width: parent.width
                Column {
                    spacing: Theme.paddingSmall
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter

                    Label {
                        text: "Bitcoin donations (click to copy)"
                        font.pixelSize: Theme.fontSizeSmall
                        anchors.horizontalCenter: parent.horizontalCenter
                    }

                    Label {
                        text:"1M8m2QjLwgQ8F5HmpgWrH63xWJbyS3jwjW"
                        font.pixelSize: Theme.fontSizeExtraSmall
                        anchors.horizontalCenter: parent.horizontalCenter

                        // workaround: TextField behaves badly in a column
                        TextField {
                            id: btAddr
                            text: parent.text
                            visible: false
                            readOnly: true
                        }
                    }

                }
                onClicked: {
                    btAddr.selectAll()
                    btAddr.copy()
                    console.log(btAddr.selectedText)
                }
            }

            Item {  height: Theme.paddingLarge ; width: parent.width }
            Rectangle { width: parent.width ; height: 1 ; color: Theme.secondaryColor }
            Item {  height: Theme.paddingLarge ; width: parent.width }

            Label {
                textFormat: Text.StyledText
                wrapMode: Text.WordWrap
                font.pixelSize: Theme.fontSizeExtraSmall
                onLinkActivated: { Qt.openUrlExternally(link) }
                width: parent.width
                text:
'Stellarium for Sailfish and Stellarium N9 Copyright (c) 2012-2015 Marko Srebre \
<br><br>
Stellarium 0.12.4 Copyright (c) 2004-2013 Fabien Chereau et al\
<br><br>\
This program is free software; you can redistribute it and/or \
modify it under the terms of the GNU General Public License \
as published by the Free Software Foundation; either version 2 \
of the License, or (at your option) any later version. \
<br> \
This program is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the \
GNU General Public License for more details. \
<br> \
You should have received a copy of the GNU General Public License \
along with this program; if not, write to the Free Software \
Foundation, Inc., 51 Franklin Street, Suite 500, Boston, MA  02110-1335, USA.\
<br>\
<a href=\"https://www.gnu.org/licenses/gpl-2.0.html\">https://www.gnu.org/licenses/gpl-2.0.html</a><br><br>'
            }
        }
    }


}
