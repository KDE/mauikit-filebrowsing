/*
 *   Copyright 2018 Camilo Higuita <milo.h@aol.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick
import QtQml

import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB

/**
 * @inherit org::mauikit::controls::PopupPage 
 * 
 * @brief A dialog to quickly select files for opening or saving.
 * 
 * This controls inherits from MauiKit PopupPage, to checkout its inherited properties refer to the docs.
 * @see MauiKit::PopupPage
 *
 * @note This component makes use of the FileBrowser.
 *
 * The FileDialog can be in two modes, one for Opening and other for Saving files.
 *
 * The file dialog allows to have multiple or single selection,
 * and filtering content specific to a file type or arbitrary name filters.
 * 
 * @image html filedialog.png
 * 
 * @code
 * Maui.Page
 *    {
 *        Maui.Controls.showCSD: true
 *        anchors.fill: parent
 *        
 *        Column
 *        {
 *            width: 100
 *            anchors.centerIn: parent            
 *            
 *            Button
 *            {
 *                text: "Open"
 *                onClicked:
 *                {
 *                    _dialog.mode = FB.FileDialog.Modes.Open
 *                    _dialog.callback = (paths) =>
 *                            {
 *                        console.log("Selected Paths", paths)
 *                        _text.text = paths.join("\n")
 *                    }
 *                    _dialog.open()
 *                }
 *            }
 *            
 *            Button
 *            {
 *                text: "Save"
 *                onClicked:
 *                {
 *                    _dialog.mode = FB.FileDialog.Modes.Save
 *                    _dialog.callback = (paths) =>
 *                            {
 *                        console.log("Save to", paths)
 *                        _text.text = paths.join("\n")
 *                    }
 *                    _dialog.open()
 *                }
 *            }
 *            
 *            Text
 *            {
 *                id: _text
 *            }            
 *        }
 *    }
 *    
 *    FB.FileDialog
 *    {
 *        id: _dialog
 *    }
 * @endcode
 *
 * <a href="https://invent.kde.org/maui/mauikit-filebrowser/examples/FileDialog.qml">You can find a more complete example at this link.</a>
 */
Maui.PopupPage
{
    id: control
    
    maxHeight: Maui.Handy.isMobile ? parent.height * 0.95 : 500
    maxWidth: 700
    
    hint: 1
    
    page.padding: 0
    closeButtonVisible: false
    headBar.visible: false
    
    /**
     * @brief The current path of the directory URL.
     * @see FileBrowser::currentPath
     */
    property alias currentPath : _browser.currentPath
    
    /**
     * @brief The FileBrowser used for listing.
     * For more details on how it works check its documentation.
     * @property FileBrowser FileDialog::browser
     */
    readonly property alias browser : _browser
    
    /**
     * @see FileBrowser::selectionBar
     * This control already has a predefined SelectionBar, and can be accessed via this alias.
     * @property MauiKit::SelectionBar FileDialog::selectionBar
     */
    readonly property alias selectionBar: _selectionBar
    
    /**
     * @brief If true then only one item can be selected, either for saving or for opening.
     * @property bool FileDialog::singleSelection
     */
    property alias singleSelection : _selectionBar.singleSelection
    
    /**
     * @brief On save mode a text field is visible and this property is used to assign its default text value.
     * By default this is set to empty string
     */
    property string suggestedFileName : ""
       
    /**
     * @brief Show the search bar to enter a search query.
     */
    property bool searchBar : false
    onSearchBarChanged: if(!searchBar) _browser.quitSearch()
    
    /**
     * @brief The two different modes to use with this dialog.
     */
    enum Modes
    {
        /**
         * To use this dialog for selecting one or multiple entries for opening.
         */
        Open,
        
        /**
         * To use this dialog to select a single directory where to save a file entry with a given name.
         */
        Save
    }
    
    /**
     * @brief The current mode in use. 
     * By default this is set to `FileDialog.Modes.Open`
     * @see Modes
     */
    property int mode : FileDialog.Modes.Open
    
    /**
     * @brief A callback function that will be invoked once the user is done selecting the files.
     * This is useful when the file dialog is going to be used for multiple purposes.
     * Otherwise you might want to use the `urlsSelected` signal.
     * @see urlsSelected
     */
    property var callback
    
    /**
     * @brief On Save mode a text field is visible, this property gives access to it.
     */
    readonly property alias textField: _textField
    
    /**
     * @brief Emitted once the URLs have been selected.
     * @param urls the selected list of URLs
     */
    signal urlsSelected(var urls)
    
    /**
     * @brief Emitted once the selection has been done.
     * @param urls the selected list of URLs
     */
    signal finished(var urls)
    
    actions:
    [
        Action
        {
            text: i18nd("mauikitfilebrowsing", "Cancel")
            onTriggered: control.close()
        },
        
        Action
        {
            text: control.mode === FileDialog.Modes.Save ? i18nd("mauikitfilebrowsing", "Save") : i18nd("mauikitfilebrowsing", "Open")
            onTriggered:
            {
                console.log("CURRENT PATHb", _browser.currentPath+"/"+textField.text)
                if(control.mode === FileDialog.Modes.Save && textField.text.length === 0)
                    return
                    
                    if(control.mode === FileDialog.Modes.Save && FB.FM.fileExists(_browser.currentPath+"/"+textField.text))
                    {
                        _confirmationDialog.open()
                    }else
                    {
                        done()
                    }
            }
        }
    ]
    
    page.footerColumn: [
        
        Maui.ToolBar
        {
            visible: control.mode === FileDialog.Modes.Save
            width: parent.width
            position: ToolBar.Footer
            
            middleContent: TextField
            {
                id: _textField
                Layout.fillWidth: true
                placeholderText: i18nd("mauikitfilebrowsing", "File name...")
                text: suggestedFileName
            }
        }
    ]
    
    Maui.InfoDialog
    {
        id: _confirmationDialog
        
        title: i18nd("mauikitfilebrowsing", "Error")
        message: i18nd("mauikitfilebrowsing", "A file named '%1' already exists!\n This action will overwrite '%1'. Are you sure you want to do this?", control.textField.text)
        template.iconSource: "dialog-warning"
        
        standardButtons: Dialog.Ok | Dialog.Cancel
        
        onAccepted: control.done()
        onRejected: close()
    }
    
    stack: Maui.SideBarView
    {
        id: pageRow
        
        Layout.fillHeight: true
        Layout.fillWidth: true
        
        sideBar.preferredWidth: 200
        sideBar.minimumWidth: 200
        sideBarContent: Loader
        {
            id: sidebarLoader
            asynchronous: true
            anchors.fill: parent
            
            sourceComponent: FB.PlacesListBrowser
            {
                onPlaceClicked: (path) =>
                {
                    //pageRow.currentIndex = 1
                    _browser.openFolder(path)
                }
                
                currentPath: _browser.currentPath
                
                list.groups:  [
                    FB.FMList.BOOKMARKS_PATH,
                    FB.FMList.REMOTE_PATH,
                    FB.FMList.CLOUD_PATH,
                    FB.FMList.DRIVES_PATH]
            }
        }
        
        Maui.Page
        {
            id: _browserLayout
            anchors.fill: parent
            
            floatingFooter: true
            flickable: _browser.flickable
            headBar.visible: true
            headerColorSet: Maui.Theme.Header
            headBar.farLeftContent: ToolButton
            {
                icon.name: pageRow.sideBar.visible ? "sidebar-collapse" : "sidebar-expand"
                onClicked: pageRow.sideBar.toggle()
                checked: pageRow.sideBar.visible
                ToolTip.delay: 1000
                ToolTip.timeout: 5000
                ToolTip.visible: hovered
                ToolTip.text: i18nd("mauikitfilebrowsing", "Toogle SideBar")
            }
            
            headBar.rightContent:[

                ToolButton
                {
                    id: searchButton
                    icon.name: "edit-find"
                    onClicked: browser.toggleSearchBar()
                    checked: browser.headBar.visible
                },

                Loader
                {
                    asynchronous: true
                    sourceComponent: Maui.ToolButtonMenu
                    {
                        icon.name: browser.settings.viewType === FB.FMList.LIST_VIEW ? "view-list-details" : "view-list-icons"

                        Maui.MenuItemActionRow
                        {
                            Action
                            {
                                icon.name: "view-hidden"
                                //                        text: i18nd("mauikitfilebrowsing", "Hidden Files")
                                checkable: true
                                checked: browser.settings.showHiddenFiles
                                onTriggered: browser.settings.showHiddenFiles = !browser.settings.showHiddenFiles
                            }

                            Action
                            {
                                icon.name: "folder-new"
                                onTriggered: browser.newItem()
                            }
                        }

                        Maui.LabelDelegate
                        {
                            width: parent.width
                            isSection: true
                            text: i18nd("mauikitfilebrowsing", "View type")
                        }

                        Action
                        {
                            text: i18nd("mauikitfilebrowsing", "List")
                            icon.name: "view-list-details"
                            checked: browser.settings.viewType === FB.FMList.LIST_VIEW
                            checkable: true
                            onTriggered:
                            {
                                if(browser)
                                {
                                    browser.settings.viewType = FB.FMList.LIST_VIEW
                                }
                            }
                        }

                        Action
                        {
                            text: i18nd("mauikitfilebrowsing", "Grid")
                            icon.name: "view-list-icons"
                            checked:  browser.settings.viewType === FB.FMList.ICON_VIEW
                            checkable: true

                            onTriggered:
                            {
                                if(browser)
                                {
                                    browser.settings.viewType = FB.FMList.ICON_VIEW
                                }
                            }
                        }

                        MenuSeparator {}

                        Maui.LabelDelegate
                        {
                            width: parent.width
                            isSection: true
                            text: i18nd("mauikitfilebrowsing", "Sort by")
                        }

                        Action
                        {
                            text: i18nd("mauikitfilebrowsing", "Type")
                            checked: browser.settings.sortBy === FB.FMList.MIME
                            checkable: true

                            onTriggered:
                            {
                                browser.settings.sortBy = FB.FMList.MIME
                            }
                        }

                        Action
                        {
                            text: i18nd("mauikitfilebrowsing", "Date")
                            checked: browser.settings.sortBy === FB.FMList.DATE
                            checkable: true

                            onTriggered:
                            {
                                browser.settings.sortBy = FB.FMList.DATE
                            }
                        }

                        Action
                        {
                            text: i18nd("mauikitfilebrowsing", "Modified")
                            checked: browser.settings.sortBy === FB.FMList.MODIFIED
                            checkable: true

                            onTriggered:
                            {
                                browser.settings.sortBy = FB.FMList.MODIFIED
                            }
                        }

                        Action
                        {
                            text: i18nd("mauikitfilebrowsing", "Size")
                            checked: browser.settings.sortBy === FB.FMList.SIZE
                            checkable: true

                            onTriggered:
                            {
                                browser.settings.sortBy = FB.FMList.SIZE
                            }
                        }

                        Action
                        {
                            text: i18nd("mauikitfilebrowsing", "Name")
                            checked:  browser.settings.sortBy === FB.FMList.LABEL
                            checkable: true

                            onTriggered:
                            {
                                browser.settings.sortBy = FB.FMList.LABEL
                            }
                        }

                        MenuSeparator{}

                        MenuItem
                        {
                            text: i18nd("mauikitfilebrowsing", "Show Folders First")
                            checked: browser.settings.foldersFirst
                            checkable: true

                            onTriggered:
                            {
                                browser.settings.foldersFirst = !browser.settings.foldersFirst
                            }
                        }

                        MenuItem
                        {
                            id: groupAction
                            text: i18nd("mauikitfilebrowsing", "Group")
                            checkable: true
                            checked: browser.settings.group
                            onTriggered:
                            {
                                browser.settings.group = !browser.settings.group
                            }
                        }
                    }
                }
            ]
            
            headBar.leftContent: Loader
                {
                    asynchronous: true
                    sourceComponent: Maui.ToolActions
                    {
                        expanded: true
                        autoExclusive: false
                        checkable: false

                        Action
                        {
                            icon.name: "go-previous"
                            onTriggered : browser.goBack()
                        }

                        Action
                        {
                            icon.name: "go-up"
                            onTriggered : browser.goUp()
                        }

                        Action
                        {
                            icon.name: "go-next"
                            onTriggered: browser.goNext()
                        }
                    }
                }
            
            footer: Maui.SelectionBar
            {
                id: _selectionBar
                
                anchors.horizontalCenter: parent.horizontalCenter
                width: Math.min(parent.width-(Maui.Style.space.medium*2), implicitWidth)
                maxListHeight: control.height - (Maui.Style.contentMargins*2)
                
                listDelegate: Maui.ListBrowserDelegate
                {
                    width: ListView.view.width
                    iconSource: model.icon
                    imageSource: model.thumbnail
                    label1.text: model.label
                    label2.text: model.url
                }
                
                onExitClicked:
                {
                    _selectionBar.clear()
                }
            }
            
            FB.FileBrowser
            {
                id: _browser
                anchors.fill: parent
                
                selectionBar: _selectionBar
                settings.viewType: FB.FMList.LIST_VIEW
                currentPath: FB.FM.homePath()
                selectionMode: control.mode === FileDialog.Modes.Open
                onItemClicked: (index) =>
                {
                    if(Maui.Handy.singleClick)
                    {
                        performAction(index)
                    }
                }
                
                onItemDoubleClicked: (index) =>
                {
                    if(!Maui.Handy.singleClick)
                    {
                        performAction(index)
                    }
                }
                
                function performAction(index)
                {
                    if(currentFMModel.get(index).isdir == "true")
                    {
                        openItem(index)
                    }
                    
                    switch(control.mode)
                    {
                        case FileDialog.Modes.Open :
                            addToSelection(currentFMModel.get(index))
                            break;
                            
                        case FileDialog.Modes.Save:
                            textField.text = currentFMModel.get(index).label
                            break;
                    }
                }
            }
        }
    }
    
    onClosed:
    {
        _selectionBar.clear()
    }
    
    /**
     * @private
     */
    function done()
    {
        var paths = _browser.selectionBar && _browser.selectionBar.visible ? _browser.selectionBar.uris : [_browser.currentPath]
        
        if(control.mode === FileDialog.Modes.Save)
        {
            for(var i in paths)
            {
                paths[i] = paths[i] + "/" + textField.text
            }
            
            //            _tagsBar.list.urls = paths
            //            _tagsBar.list.updateToUrls(_tagsBar.getTags())
        }
        
        control.finished(paths)
        
        //        if(control.mode === modes.SAVE) //do it after finished in cas ethe files need to be saved aka exists, before tryign to insert tags
        //        {
        //            _tagsBar.list.urls = paths
        //            _tagsBar.list.updateToUrls(_tagsBar.getTags())
        //        }
        
        if(control.callback)
        {
            control.callback(paths)
        }
        
        control.urlsSelected(paths)
        control.close()
    }
}
