/*
* Copyright (C) 2008-2012 J-P Nurmi <jpnurmi@gmail.com>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

import QtQuick 1.1
import com.nokia.symbian 1.1
import "UIConstants.js" as UI

CommonDialog {
    id: root

    property int selectedIndex: -1
    property alias model: listView.model

    privateCloseIcon: true

    content: ListView {
        id: listView

        clip: true
        currentIndex : -1
        width: root.platformContentMaximumWidth
        height: Math.min(privateStyle.dialogMaxSize, count * privateStyle.menuItemHeight)

        delegate: MenuItem {
            platformInverted: root.platformInverted
            text: modelData
            privateSelectionIndicator: selectedIndex == index

            onClicked: {
                selectedIndex = index
                root.accept()
            }

            Keys.onPressed: {
                if (event.key == Qt.Key_Up || event.key == Qt.Key_Down)
                    scrollBar.flash()
            }
        }

        Keys.onPressed: {
            if (event.key == Qt.Key_Up || event.key == Qt.Key_Down
                || event.key == Qt.Key_Left || event.key == Qt.Key_Right
                || event.key == Qt.Key_Select || event.key == Qt.Key_Enter
                || event.key == Qt.Key_Return) {
                symbian.listInteractionMode = Symbian.KeyNavigation
                listView.currentIndex = 0
                event.accepted = true
            }
        }

        ScrollBar {
            id: scrollBar
            flickableItem: listView
            interactive: false
            visible: listView.contentHeight > listView.height
            anchors { top: listView.top; right: listView.right }
            platformInverted: root.platformInverted
        }
    }

    onClickedOutside: {
        privateStyle.play(Symbian.PopUp)
        reject()
    }

    onStatusChanged: {
        if (status == DialogStatus.Opening) {
            if (listView.currentItem)
                listView.currentItem.focus = false
            listView.currentIndex = -1
            listView.positionViewAtIndex(0, ListView.Beginning)
        } else if (status == DialogStatus.Open) {
            listView.focus = true
            if (listView.contentHeight > listView.height)
                scrollBar.flash(Symbian.FadeInFadeOut)
        }
    }
}
