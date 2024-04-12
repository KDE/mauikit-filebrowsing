/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2020  camilo <chiguitar@unal.edu.co>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick
import QtQuick.Layouts 

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB


/**
 * @inherit org::mauikit::control::PopupPage
 * A dialog with a list of services -that can handled the associated list of URLs.
 * @see urls
 *
 * This control inherits from MauiKit PopupPage, to checkout its inherited properties refer to docs.
 * 
 * The services listed can open the file type of the file URLs.
 * 
 * @image html openwithdialog.png "Example"
 * 
 * @code
 * Maui.Page
 * {
 *    Maui.Controls.showCSD: true
 *    anchors.fill: parent
 * 
 *    Button
 *    {
 *        anchors.centerIn: parent
 *        text: "Open"
 *        onClicked: _dialog.open()
 *    }
 * 
 *    FB.OpenWithDialog
 *    {
 *        id: _dialog
 *        urls: ["/home/camiloh/Pictures/blend_by_rashadisrazzi_d8ttd59/2K.png"]
 *    }
 * }
 * @endcode
 *
 * <a href="https://invent.kde.org/maui/mauikit-filebrowser/examples/OpenWithDialog.qml">You can find a more complete example at this link.</a>
 */
Maui.PopupPage
{
    id: control
    
    /**
     * @brief List of file URLs to look for associated services.
     * @property var OpenWithDialog::urls
     */
    property alias urls : _openWithList.urls
    
    widthHint: 0.9
    maxWidth: 350
    persistent: false
    
    page.title: i18nd("mauikitfilebrowsing", "Open with")
    headBar.visible: true
    
    stack: Maui.ListBrowser
    {
        id: _list
        Layout.fillWidth: true
        Layout.fillHeight: true
        
        model: Maui.BaseModel
        {
            list: FB.OpenWithModel
            {
                id: _openWithList
            }
        }
        
        delegate: Maui.ListBrowserDelegate
        {
            width: ListView.view.width
            hoverEnabled: true
            
            label1.text: model.label
            label2.text: model.comment
            
            iconSource: model.icon
            iconSizeHint: Maui.Style.iconSizes.big
            
            onClicked:
            {
                _list.currentIndex = index
                _openWithList.openWith(index)
                close()
            }
        }
    }  
}
