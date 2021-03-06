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

import QtQuick 2.14
import QtQml 2.14

import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.3 as FB

import "private" as Private

/*!
 * A control to list and browse the file system, with convinient properties
 * for filtering and sorting its contents
 * 
 * There are three different possible ways to display the contents: Grid, List and Miller views.
 * Some basic file item actions are implemented by default, like copy, cut, rename and remove.
 * 
 * This component functionality can be easily expanded to be more feature rich.
 */
Maui.Page
{
    id: control
    
    onGoBackTriggered: control.goBack()
    onGoForwardTriggered: control.goForward()
    
    title: view.title
    focus: true
    flickable: control.currentView.flickable
    floatingFooter: false
    
    showTitle: false
    
    /*!
     *      \qmlproperty FileBrowser::currentPath
     * 
     *      The current path of the directory URL.
     *      To list a directory path, or other location, use the right schemas,
     *      some of them are file://, webdav://, trash:///, tags://
     */
    property alias currentPath : _browser.path
    onCurrentPathChanged : _searchField.clear()
    
    /*!
     *      \qmlproperty BrowserSettings FileBrowser::settings
     * 
     *      A group of properties for controlling the sorting, listing and behaviour of the file browser.
     *      For more details check the BrowserSettings documentation.
     */
    property alias settings : _browser.settings
    
    /*!
     *      \qmlproperty Item FileBrowser::view
     * 
     *      The browser can be in two different view states: the file browsing or the search view, this
     *      property gives access to the current view in use.
     */
    property alias view : _stackView.currentItem
    
    property alias browser : _browser
    /*!
     *      \qmlproperty DropArea FileBrowser::dropArea
     * 
     *      Drop area component, for dropping files.
     *      By default sonme drop actions are handled, for other type of uris this property can be used to handle those.
     */
    property alias dropArea : _dropArea
    
    /*!
     *      \qmlproperty int FileBrowser::currentIndex
     * 
     *      Current index of the item selected in the file browser.
     */
    property int currentIndex  : -1
    
    Binding on currentIndex
    {
        value: currentView.currentIndex
        //         restoreMode: Binding.RestoreBindingOrValue
    }
    
    /*!
     *      \qmlproperty Item FileBrowser::currentView
     * 
     *      Current view of the file browser. Possible views are List = ListBrowser
     *      Grid = GridView
     *      Miller = ListView
     */
    readonly property QtObject currentView : _stackView.currentItem.browser
    
    /*!
     *      The file browser model list controller being used. The List and Grid views use the same FMList, the
     *      Miller columns use several different models, one for each column.
     */
    readonly property FB.FMList currentFMList : view.currentFMList
    
    /*!
     *      The file browser data model being used. The List and Grid views use the same model, the
     *      Miller columns use several different FMList controllers, one for each column.
     */
    readonly property Maui.BaseModel currentFMModel : view.currentFMModel
    
    /*!
     *      isSearchView : bool
     *      If the file browser current view is the search view.
     */
    readonly property bool isSearchView : _stackView.currentItem.objectName === "searchView"
    
    /*!
     *      If the file browser enters selection mode, allowing the selection of multiple items.
     */
    property bool selectionMode: false
    
    /*!
     *      \qmlproperty int FileBrowser::gridItemSize
     * 
     *      Size of the items in the grid view. The size is for the combined thumbnail/icon and the title label.
     */
    property alias gridItemSize : _browser.gridItemSize
    
    /*!
     *      \qmlproperty int FileBrowser::listItemSize
     * 
     *      Size of the items in the grid view. The size is for the combined thumbnail/icon and the title label.
     */
    property alias listItemSize : _browser.listItemSize
    
    /*!
     *     \qmlproperty var FileBrowser::indexHistory
     * 
     *     History of the items indexes.
     */
    property var indexHistory : []
    
    // need to be set by the implementation as features
    /*!
     * 
     */
    property Maui.SelectionBar selectionBar : null //TODO remove
    
    
    //access to the loaded the dialog components
    /*!
     *      \qmlproperty Dialog FileBrowser::dialog
     *      The message and action dialogs are loaded when needed.
     *      This property gives access to the current dialog opened.
     */
    property alias dialog : dialogLoader.item
    
    
    //signals
    /*!
     *      An item was clicked.
     */
    signal itemClicked(int index)
    
    /**
     *     An item was double clicked.
     */
    signal itemDoubleClicked(int index)
    
    /*!
     *      An item was right clicked, on mobile devices this is translated from a long press and relase.
     */
    signal itemRightClicked(int index)
    
    /*!
     *      The left emblem of the item was clicked.
     */
    signal itemLeftEmblemClicked(int index)
    
    /*!
     *      The right emblem of the item was clicked.
     */
    signal itemRightEmblemClicked(int index)
    
    /*!
     *      The file browser empty area was right clicked.
     */
    signal rightClicked()
    
    /*!
     *      A key, physical or not, was pressed.
     *      The event contains the relevant information.
     */
    signal keyPress(var event)
    
    /*!
     *      File URLS were dropped onto the file browser area.
     */
    signal urlsDropped(var urls)
    
    headBar.forceCenterMiddleContent: isWide
    headBar.visible: control.settings.searchBarVisible
    headBar.leftContent: Loader
    {
        asynchronous: true
        active: control.isSearchView
        visible: active
        sourceComponent: ToolButton
        {
            text: i18n("Back")
            icon.name: "go-previous"
            onClicked: control.quitSearch()
        }
    }
    
    headBar.middleContent: Maui.SearchField
    {
        id: _searchField
        focus: true
        Layout.fillWidth: true
        Layout.maximumWidth: 500
        Layout.alignment: Qt.AlignCenter
        placeholderText: _filterButton.checked ? i18n("Filter") : ("Search")
        inputMethodHints: Qt.ImhNoAutoUppercase
        
        onAccepted:
        {
            if(_filterButton.checked)
            {
                if(text.includes(","))
                {
                    control.view.filters = text.split(",")
                }else
                {
                    control.view.filter = text
                }
                
            }else
            {
                control.search(text)
            }
        }
        
        onCleared:
        {
            if(_filterButton.checked)
            {
                control.currentFMModel.clearFilters()
            }
        }
        
        onTextChanged:
        {
            if(_filterButton.checked)
                _searchField.accepted()                
        }
        
        Keys.enabled: _filterButton.checked
        Keys.onPressed:
        {
            // Shortcut for clearing selection
            if(event.key == Qt.Key_Up)
            {
                control.currentView.forceActiveFocus()
            }
        }
        
        actions: Action
        {
            id: _filterButton
            icon.name: "view-filter"
            //            text: i18n("Filter")
            checkable: true
            checked: true
            onTriggered:
            {
                control.view.filter = ""
                _searchField.clear()
                _searchField.forceActiveFocus()
            }
        }
    }
    
    footBar.visible: control.currentPath.startsWith("trash:/")
    
    footerPositioning: ListView.InlineFooter
    
    footBar.rightContent: Button
    {
        visible: control.currentPath.startsWith("trash:/")
        icon.name: "trash-empty"
        text: i18n("Empty Trash")
        onClicked: FB.FM.emptyTrash()
    }
    
    Loader
    {
        id: dialogLoader
    }
    
    Component
    {
        id: removeDialogComponent
        
        Maui.FileListingDialog
        {
            id: _removeDialog
            property double freedSpace : calculateFreedSpace(urls)
            title:  i18n("Removing %1 files", urls.length)
            message: i18n("Delete %1  \nTotal freed space %2", (Maui.Handy.isLinux ? "or move to trash?" : "? This action can not be undone."),  Maui.Handy.formatSize(freedSpace)) 
            rejectButton.text: i18n("Delete")
            acceptButton.text: i18n("Trash")
            acceptButton.visible: Maui.Handy.isLinux            
            
            actions: Action
            {
                text: i18n("Cancel")
                onTriggered: _removeDialog.close()
            }
            
            onRejected:
            {
                FB.FM.removeFiles(urls)
                close()
            }
            
            onAccepted:
            {
                FB.FM.moveToTrash(urls)
                close()
            }
            
            function calculateFreedSpace(urls)
            {
                var size = 0
                for(var url of urls)
                {
                    size += parseFloat(FB.FM.getFileInfo(url).size)
                }
                
                return size
            }
        }
    }
    
    Component
    {
        id: newDialogComponent
        
        Maui.NewDialog
        {
            id: _newDialog
            title: i18n("New %1", _newActions.currentIndex === 0 ? "folder" : "file" )
            message: i18n("Create a new folder or a file with a custom name")
            acceptButton.text: i18n("Create")
            onFinished:
            {
                switch(_newActions.currentIndex)
                {
                    case 0: control.currentFMList.createDir(text); break;
                    case 1: FB.FM.createFile(control.currentPath, text); break;                    
                }
            }
            
            textEntry.placeholderText: i18n("Name")
            
            Maui.ToolActions
            {
                id: _newActions
                expanded: true
                autoExclusive: true
                display: ToolButton.TextBesideIcon
                currentIndex: String(_newDialog.textEntry.text).indexOf(".") > 0 ? 1 : 0
                
                Action
                {
                    icon.name: "folder-new"
                    text: i18n("Folder")
                }
                
                Action
                {
                    icon.name: "document-new"
                    text: i18n("File")
                }
            }
        }
    }
    
    Component
    {
        id: renameDialogComponent
        
        Maui.NewDialog
        {
            id: _renameDialog
            property var item : ({})
            title: i18n("Rename")
            message: i18n("Change the name of a file or folder")
            template.iconSource: item.icon
            template.imageSource: item.thumbnail
            template.iconSizeHint: Maui.Style.iconSizes.huge
            textEntry.text: item.label
            textEntry.placeholderText: i18n("New name")
            onFinished: FB.FM.rename(item.path, textEntry.text)
            onRejected: close()
            acceptButton.text: i18n("Rename")
            rejectButton.text: i18n("Cancel")
            
            onOpened:
            {
                item = control.currentFMModel.get(control.currentIndex)
                
                if(_renameDialog.textEntry.text.lastIndexOf(".") >= 0)
                {                    
                    _renameDialog.textEntry.select(0, _renameDialog.textEntry.text.lastIndexOf("."))
                }else
                {                    
                    _renameDialog.textEntry.selectAll()
                }                
            }
            
            //             template.iconComponent: Maui.GridItemTemplate
            //             {
            //                 anchors.fill: parent
            //                 iconSource: _renameDialog.template.iconSource
            //                 imageSource: _renameDialog.template.imageSource
            //                 label1.text: _renameDialog.textEntry.text
            //             }
        }
    }
    
    Component
    {
        id: _newTagDialogComponent        
        FB.NewTagDialog {}
    }
    
    property string typingQuery
    
    Maui.Chip
    {
        z: control.z + 99999
        Maui.Theme.colorSet:Maui.Theme.Complementary
        visible: _typingTimer.running
        label.text: typingQuery
        anchors.left: parent.left
        anchors.top: parent.top
        showCloseButton: false
        anchors.margins: Maui.Style.space.medium
    }
    
    Timer
    {
        id: _typingTimer        
        interval: 250        
        onTriggered:
        {
            const index = control.currentFMList.indexOfName(typingQuery)
            if(index > -1)
            {
                console.log("FOUDN TRYPIGN IDNEX", index)
                control.currentIndex = control.currentFMModel.mappedFromSource(index)
            }
            
            typingQuery = ""
        }
    }
    
    Connections
    {
        target: control.currentView
        ignoreUnknownSignals: true
        
        function onKeyPress(event)
        {
            const index = control.currentIndex
            const item = control.currentFMModel.get(index)            
            
            var pat = /^([a-zA-Z0-9 _-]+)$/
            
            if(event.count === 1 && pat.test(event.text))
            {
                typingQuery += event.text
                _typingTimer.restart()   
                event.accepted = true
            }
            
            // Shortcuts for refreshing
            if((event.key === Qt.Key_F5))
            {
                control.currentFMList.refresh()
                event.accepted = true
            }
            
            // Shortcuts for renaming
            if((event.key === Qt.Key_F2))
            {
                dialogLoader.sourceComponent = renameDialogComponent
                dialog.open()
                event.accepted = true
            }
            
            // Shortcuts for selecting file
            if((event.key === Qt.Key_A) && (event.modifiers & Qt.ControlModifier))
            {
                control.selectAll()
                event.accepted = true
            }
            
            if((event.key === Qt.Key_Left || event.key === Qt.Key_Right || event.key === Qt.Key_Down || event.key === Qt.Key_Up) && (event.modifiers & Qt.ControlModifier) && (event.modifiers & Qt.ShiftModifier))
            {
                if(control.selectionBar && control.selectionBar.contains(item.path))
                {
                    control.selectionBar.removeAtUri(item.path)
                }else
                {
                    control.addToSelection(item)
                }
                //event.accepted = true
            }
            
            //shortcut for opening files
            if(event.key === Qt.Key_Return)
            {
                indexHistory.push(index)
                control.openItem(index)
                event.accepted = true
            }
            
            // Shortcut for pasting an item
            if((event.key == Qt.Key_V) && (event.modifiers & Qt.ControlModifier))
            {
                control.paste(Maui.Handy.getClipboard().urls)
                event.accepted = true
            }
            
            // Shortcut for cutting an item
            if((event.key == Qt.Key_X) && (event.modifiers & Qt.ControlModifier))
            {
                const urls = filterSelection(control.currentPath, item.path)
                control.cut(urls)
                event.accepted = true
            }
            
            // Shortcut for copying an item
            if((event.key == Qt.Key_C) && (event.modifiers & Qt.ControlModifier))
            {
                const urls = filterSelection(control.currentPath, item.path)
                control.copy(urls)
                event.accepted = true
            }
            
            // Shortcut for removing an item
            if(event.key === Qt.Key_Delete)
            {
                const urls = filterSelection(control.currentPath, item.path)
                control.remove(urls)
                event.accepted = true
            }
            
            // Shortcut for going back in browsing history
            if(event.key === Qt.Key_Backspace || event.key == Qt.Key_Back)
            {
                if(control.selectionBar && control.selectionBar.count> 0)
                {
                    control.selectionBar.clear()
                }
                else
                {
                    control.goBack()
                }
                event.accepted = true
            }
            
            // Shortcut for clearing selection and filtering
            if(event.key === Qt.Key_Escape) //TODO not working, the event is not catched or emitted or is being accepted else where?
            {
                if(control.selectionBar && control.selectionBar.count > 0)
                {
                    control.selectionBar.clear()
                }
                
                control.view.filter = ""
                event.accepted = true
            }
            
            //Shortcut for opening filtering
            if((event.key === Qt.Key_F) && (event.modifiers & Qt.ControlModifier))
            {
                control.toggleSearchBar()                
                event.accepted = true
            }
            
            control.keyPress(event)
        }
        
        function onItemsSelected(indexes)
        {
            if(indexes.length)
            {
                control.currentIndex = indexes[0]
                control.selectIndexes(indexes)
            }
        }
        
        function onItemClicked(index)
        {
            control.currentIndex = index
            indexHistory.push(index)
            control.itemClicked(index)
            control.currentView.forceActiveFocus()
        }
        
        function onItemDoubleClicked(index)
        {
            control.currentIndex = index
            indexHistory.push(index)
            control.itemDoubleClicked(index)
            control.currentView.forceActiveFocus()
        }
        
        function onItemRightClicked(index)
        {
            control.currentIndex = index
            control.itemRightClicked(index)
            control.currentView.forceActiveFocus()
        }
        
        function onItemToggled(index)
        {
            const item = control.currentFMModel.get(index)
            
            if(control.selectionBar && control.selectionBar.contains(item.path))
            {
                control.selectionBar.removeAtUri(item.path)
            }else
            {
                control.addToSelection(item)
            }
            control.itemLeftEmblemClicked(index)
            control.currentView.forceActiveFocus()
        }
        
        
        function onAreaClicked(mouse)
        {
            if(!Maui.Handy.isMobile && mouse.button === Qt.RightButton)
            {
                control.rightClicked()
            }
            
            control.currentView.forceActiveFocus()
        }
        
        //        function onAreaRightClicked(mouse)
        //        {
        //            control.rightClicked()
        //            control.currentView.forceActiveFocus()
        //        }
    }   
    
    Maui.ContextualMenu
    {
        id: _dropMenu
        property string urls
        //enabled: FB.FM.getFileInfo(control.currentPath).isdir == "true"

        MenuItem
        {
            text: i18n("Copy here")
            icon.name: "edit-copy"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                FB.FM.copy(urls, control.currentPath, false)
            }
        }

        MenuItem
        {
            text: i18n("Move here")
            icon.name: "edit-move"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                FB.FM.cut(urls, control.currentPath)
            }
        }

        MenuItem
        {
            text: i18n("Link here")
            icon.name: "edit-link"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                for(var i in urls)
                    FB.FM.createSymlink(urls[i], control.currentPath)
            }
        }

        MenuSeparator {}

        MenuItem
        {
            text: i18n("Cancel")
            icon.name: "dialog-cancel"
            onTriggered: _dropMenu.close()
        }
    }

    Maui.StackView
    {
        id: _stackView
        anchors.fill: parent
        
        initialItem: DropArea
        {
            id: _dropArea
            property alias browser : _browser
            property alias currentFMList : _browser.currentFMList
            property alias currentFMModel: _browser.currentFMModel
            property alias filter: _browser.filter
            property alias filters: _browser.filters
            property alias title : _browser.title
            
            onDropped:
            {
                if(drop.urls)
                {
                    _dropMenu.urls = drop.urls.join(",")
                    _dropMenu.show()
                    control.urlsDropped(drop.urls)
                }
            }
            
            opacity:  _dropArea.containsDrag ? 0.5 : 1

            Private.BrowserView
            {
                id: _browser                
                anchors.fill: parent
                selectionMode: control.selectionMode
                
                Binding on currentIndex
                {
                    value: control.currentIndex
                    restoreMode: Binding.RestoreBindingOrValue
                }
                
                Loader
                {
                    active: control.currentPath === "tags://" ||  control.currentPath === "tags:///"
                    visible: active
                    asynchronous: true
                    
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                    anchors.rightMargin: Maui.Style.toolBarHeight
                    anchors.bottomMargin: Maui.Style.toolBarHeight + control.flickable.bottomMargin
                    
                    sourceComponent: Maui.FloatingButton
                    {
                        icon.name : "list-add"
                        onClicked: 
                        {
                            dialogLoader.sourceComponent = _newTagDialogComponent
                            dialog.open()
                        }
                    }
                }
            }
         }
        
        Component
        {
            id: _searchBrowserComponent
            
            Private.BrowserView
            {
                id: _searchBrowser
                property alias browser : _searchBrowser
                
                Binding on currentIndex
                {
                    value: control.currentIndex
                    restoreMode: Binding.RestoreBindingOrValue
                }
                
                objectName: "searchView"
                gridItemSize: control.gridItemSize
                listItemSize: control.listItemSize
                settings.viewType: control.settings.viewType 
            }
        }
    }
    
    Component.onCompleted:
    {
        control.currentView.forceActiveFocus()
    }    
    
    /**
     * 
     **/
    function copy(urls)
    {
        if(urls.length <= 0)
        {
            return
        }
        
        Maui.Handy.copyToClipboard({"urls": urls}, false)
    }
    
    /**
     * 
     **/
    function cut(urls)
    {
        if(urls.length <= 0)
        {
            return
        }
        
        Maui.Handy.copyToClipboard({"urls": urls}, true)
    }
    
    /**
     * 
     **/
    function paste()
    {
        const data = Maui.Handy.getClipboard()
        const urls = data.urls
        
        if(!urls)
        {
            return
        }
        if(data.cut)
        {
            control.currentFMList.cutInto(urls)
        }else
        {
            control.currentFMList.copyInto(urls)
        }
    }
    
    /**
     * 
     **/
    function remove(urls)
    {
        if(urls.length <= 0)
        {
            return
        }
        
        dialogLoader.sourceComponent= removeDialogComponent
        dialog.urls = urls
        dialog.open()
    }
    
    /**
     * 
     **/
    function openItem(index)
    {
        const item = control.currentFMModel.get(index)
        const path = item.path
        
        switch(control.currentFMList.pathType)
        {
            case FB.FMList.CLOUD_PATH: //TODO deprecrated and needs to be removed or clean up for 1.1
                if(item.isdir === "true")
                {
                    control.openFolder(path)
                }
                else
                {
                    FB.FM.openCloudItem(item)
                }
                break;
            default:
                if(control.selectionMode && item.isdir == "false")
                {
                    if(control.selectionBar && control.selectionBar.contains(item.path))
                    {
                        control.selectionBar.removeAtPath(item.path)
                    }else
                    {
                        control.addToSelection(item)
                    }
                }
                else
                {
                    if(item.isdir == "true")
                    {
                        control.openFolder(path)
                    }
                    else
                    {
                        control.openFile(path)
                    }
                }
        }
    }
    
    /**
     * 
     **/
    function openFile(path)
    {
        FB.FM.openUrl(path)
    }
    
    /**
     * 
     **/
    function openFolder(path)
    {
        if(!String(path).length)
        {
            return;
        }
        
        if(control.isSearchView)
        {
            control.quitSearch()
        }
        
        control.currentPath = path
        _browser.forceActiveFocus()
    }
    
    /**
     * 
     **/
    function goBack()
    {
        openFolder(control.currentFMList.previousPath())
        //        control.currentIndex = indexHistory.pop()
    }
    
    /**
     * 
     **/
    function goForward()
    {
        openFolder(control.currentFMList.posteriorPath())
    }
    
    /**
     * 
     **/
    function goUp()
    {
        openFolder(control.currentFMList.parentPath)
    }
    
    /**
     * For this to work the implementation needs to have passed a selectionBar
     **/
    function addToSelection(item)
    {
        if(control.selectionBar == null || item.path.startsWith("tags://") || item.path.startsWith("applications://"))
        {
            return
        }
        
        control.selectionBar.append(item.path, item)
    }
    
    
    /**
     * Given a list of indexes add them to the selectionBar
     **/
    function selectIndexes(indexes)
    {
        if(control.selectionBar == null)
        {
            return
        }
        
        for(var i in indexes)
            addToSelection(control.currentFMModel.get(indexes[i]))
    }
    
    /**
     * 
     **/
    function selectAll() //TODO for now dont select more than 100 items so things dont freeze or break
    {
        if(control.selectionBar == null)
        {
            return
        }
        
        selectIndexes([...Array( Math.min(control.currentFMList.count, 100)).keys()])
    }
    
    /**
     * 
     **/
    function bookmarkFolder(paths) //multiple paths
    {
        for(var i in paths)
        {
            FB.FM.bookmark(paths[i])
        }
    }    
    
    function toggleSearchBar() //only opens the searchbar toolbar, not the search view page
    {
        if(control.settings.searchBarVisible)
        {
            control.settings.searchBarVisible = false
            quitSearch()
            _browser.forceActiveFocus()
        }else
        {
            control.settings.searchBarVisible = true
            _searchField.forceActiveFocus()
        }
    }
    
    /**
     * 
     **/
    function openSearch() //opens the search view and focuses the search field
    {
        if(!control.isSearchView)
        {
            _stackView.push(_searchBrowserComponent)
        }
        control.settings.searchBarVisible = true
        _searchField.forceActiveFocus()
    }
    
    /**
     * 
     **/
    function quitSearch()
    {
        _stackView.pop()
        _browser.forceActiveFocus()
    }
    
    /**
     * 
     **/
    function search(query)
    {
        openSearch()
        _searchField.text = query
        _stackView.currentItem.title = i18n("Search: %1", query)
        _stackView.currentItem.currentFMList.search(query, _browser.currentFMList)
        _stackView.currentItem.forceActiveFocus()
    }
    
    /**
     * 
     **/
    function newItem()
    {
        dialogLoader.sourceComponent= newDialogComponent
        dialog.open()
        dialog.forceActiveFocus()
    }
    
    /**
     * 
     **/
    function renameItem()
    {
        dialogLoader.sourceComponent= renameDialogComponent
        dialog.open()
        dialog.forceActiveFocus()
    }
    
    /**
     * 
     **/
    function removeItem()
    {
        dialogLoader.sourceComponent= renameDialogComponent
        dialog.open()
        dialog.forceActiveFocus()
    }
    
    /**
     * Filters the content of the selection to the current path. The currentPath must be a directory, so the selection can be compared if it is its parent directory. The itemPath is a default item path in case the selectionBar is empty
     **/
    function filterSelection(currentPath, itemPath)
    {
        var res = []
        
        if(selectionBar && selectionBar.count > 0 && selectionBar.contains(itemPath))
        {
            const uris = selectionBar.uris
            for(var uri of uris)
            {
                if(String(FB.FM.parentDir(uri)) === currentPath)
                {
                    res.push(uri)
                }
            }
            
        } else
        {
            res = [itemPath]
        }
        
        return res
    }
    
    function forceActiveFocus()
    {
        control.currentView.forceActiveFocus()
    }
}
