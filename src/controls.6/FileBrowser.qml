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

import "private" as Private

/**
 * @inherit MauiKit::Controls::Page
 * @brief This control displays the file system entries in a given directory, allows subsequent browsing of the file hierarchy and exposes basic file handling functionality.
 *
 * This controls inherits from MauiKit Page, to checkout its inherited properties refer to the docs.
 * @see MauiKit::Page
 *
 * @image html filebrowser2.png "FileBrowser control"
 *
 * @section features Features
 *
 * This control exposes a series of convenient properties and functions for filtering and sorting, for creating new entries, perform searches, and modify the entries.
 *
 * There are two different possible ways to display the contents: Grid and List, using the `settings.viewType` property.
 * @see FMList::VIEW_TYPE
 *
 * More browsing properties can be tweaked via the exposed `settings` alias property.
 * @see settings
 *
 * Some basic file item actions are implemented by default, like copy, cut, rename and remove, and exposed as methods.
 * @see copy
 * @see cut
 * @see paste
 * @see remove
 *
 * @note This component functionality can be easily expanded to be more feature rich, see for example the Maui Index application.
 *
 * This control allows to perform recursive searches, or simple filtering of elements in the current location by using the search bar. Besides these two, the user can quickly start fly-typing within the browser, to jump quickly to a matching entry.
 *
 * @subsection shorcuts Shortcuts
 * This control supports multiple keyboard shortcuts, for selecting multiple elements, creating new entries, copying, deleting and renaming files.
 *
 * The supported keyboard shortcuts are:
 *
 * - `Ctrl + N` Create a new entry
 * - `Ctrl + F` Open the search bar
 * - `F5`  Refresh/reload the content
 * - `F2` Rename an entry
 * - `Ctrl + A` Select all the entries
 * - `Ctrl + N` Create a new entry
 * - `Ctrl + Shift + ← ↑ → ↓` Select items in any direction
 * - `Ctrl + Return` Open an entry
 * - `Ctrl + V` Paste element in the clipboard to the current location
 * - `Ctrl + X` Cut an entry
 * - `Ctrl + C` Copy an entry
 * - `Ctrl + Delete` Remove an entry
 * - `Ctrl + Backspace` Clear selection/Go back to previous location
 * - `Ctrl + Esc` Clear the selection/Clear the filter
 *
 * @section structure Structure
 *
 * This control inherits from MauiKit Page, so it has a header and footer. The header bar has a search field for performing searches, by default it is hidden.
 *
 * The footer bar by default has contextual buttons, depending on the current location. For example, for the trash location, it has contextual actions for emptying the trash can, etc.
 *
 * @warning It is recommended to not add elements to the footer or header bars, instead - if needed - wrap this control into a MauiKit Page, and utilize its toolbars.
 *
 * @code
 * FB.FileBrowser
 * {
 *    Maui.Controls.showCSD: true
 *    headBar.visible: true
 *    anchors.fill: parent
 *    currentPath: FB.FM.homePath()
 *    settings.viewType: FB.FMList.GRID_VIEW
 * }
 * @endcode
 *
 * <a href="https://invent.kde.org/maui/mauikit-filebrowser/examples/FileBrowser.qml">You can find a more complete example at this link.</a>
 *
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

    /**
     * @brief The current URL path for the directory or location.
     * To list a directory path, or other location, use the right schema, some of them are `file://`, `webdav://`, `trash://`/, `tags://`, `fish://`.
     * By default a URL without a well defined schema will be assumed as a local file path.
     *
     * @note KDE KIO is used as the back-end technology, so any of the supported plugins by KIO will also work here.
     *
     * @property string FileBrowser::currentPath
     */
    property alias currentPath : _browser.path
    onCurrentPathChanged : _searchField.clear()

    /**
     * @brief A group of properties for controlling the sorting, listing and other behaviour of the browser.
     * For more details check the BrowserSettings documentation.
     * @see BrowserSettings
     *
     * @code
     * settings.onlyDirs: true
     * settings.sortBy: FB.FMList.LABEL
     * settings.showThumbnails: false
     * @endcode
     *
     * @property BrowserSettings FileBrowser::settings
     */
    readonly property alias settings : _browser.settings

    /**
     * @brief The browser could be in two different view states: [1]the file browsing or [2]the search view.
     * This property gives access to the current view in use.
     */
    readonly property alias view : _stackView.currentItem

    /**
     * @brief An alias to the control listing the entries.
     * This component is handled by BrowserView, and exposed for fine tuning its properties
     * @property BrowserView FileBrowser::browser
     */
    readonly property alias browser : _browser

    /**
     * @brief Drop area component, for dropping files.
     * By default some of the drop actions are handled, for other type of URIs this alias can be used to handle those.
     *
     * The urlsDropped signal is emitted once one or multiple valid file URLs are dropped, and a popup contextual menu is opened with the supported default actions.
     * @property DropArea FileBrowser::dropArea
     */
    readonly property alias dropArea : _dropArea

    /**
     * @brief Current index of the item selected in the file browser.
     */
    property int currentIndex  : -1

    Binding on currentIndex
    {
        value: currentView.currentIndex
    }

    /**
     *  @brief Current view of the file browser. Possible views are:
     * - List = MauiKit::ListBrowser
     * - Grid = MauiKit::GridBrowser
     * @property Item FileBrowser::currentView
     */
    readonly property QtObject currentView : _stackView.currentItem.browser

    /**
     * @brief The file browser model list.
     * The List and Grid views use the same FMList.
     * @see FMList
     */
    readonly property FB.FMList currentFMList : view.currentFMList

    /**
     * @brief The file browser model controller. This is the controller which allows for filtering, index mapping, and sorting.
     * @see MauiModel
     */
    readonly property Maui.BaseModel currentFMModel : view.currentFMModel

    /**
     * @brief Whether the file browser current view is the search view.
     */
    readonly property bool isSearchView : _stackView.currentItem.objectName === "searchView"

    /**
     * @brief Whether the file browser enters selection mode, allowing the selection of multiple items.
     * This will make all the browsing entries checkable.
     * @property bool FileBrowser::selectionMode
     */
    property alias selectionMode: _browser.selectionMode

    /**
     * @brief Size of the items in the grid view.
     * The size is for the combined thumbnail/icon and the title label.
     *
     * @property int FileBrowser::gridItemSize
     */
    property alias gridItemSize : _browser.gridItemSize

    /**
     * @brief Size of the items in the list view.
     * @note The size is actually taken as the size of the icon thumbnail. And it is mapped to the nearest size of the standard icon sizes.
     * @property int FileBrowser::listItemSize
     */
    property alias listItemSize : _browser.listItemSize

    /**
     * @brief The SelectionBar to be used for adding items to the selected group.
     * This is optional, but if it is not set, then some feature will not work, such as selection mode, selection shortcuts etc.
     *
     * @see MauiKit::SelectionBar
     *
     * @code
     * Maui.Page
{
    Maui.Controls.showCSD: true
    anchors.fill: parent
    floatingFooter: true

    FB.FileBrowser
    {
        Maui.Controls.showCSD: true

        anchors.fill: parent
        currentPath: FB.FM.homePath()
        settings.viewType: FB.FMList.GRID_VIEW
        selectionBar: _selectionBar
    }

    footer: Maui.SelectionBar
    {
        id: _selectionBar
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(parent.width-(Maui.Style.space.medium*2), implicitWidth)
        maxListHeight: root.height - (Maui.Style.contentMargins*2)

        Action
        {
            icon.name: "love"
            onTriggered: console.log(_selectionBar.getSelectedUrisString())
        }

        Action
        {
            icon.name: "folder"
        }

        Action
        {
            icon.name: "list-add"
        }

        onExitClicked: clear()
    }
}
     * @endcode
     */
    property Maui.SelectionBar selectionBar : null

    /**
     * @brief An alias to the currently loaded dialog, if not dialog is loaded then null.
     *
     * @property Dialog FileBrowser::dialog
     */
    readonly property alias dialog : dialogLoader.item

    /**
     * @brief Whether the browser is on a read only mode, and modifications are now allowed, such as pasting, moving, removing or renaming.
     * @property bool FileBrowser::readOnly
     */
    property alias readOnly : _browser.readOnly


    /**
     * @brief Emitted when an item has been clicked.
     * @param index the index position of the item
     */
    signal itemClicked(int index)

    /**
     * @brief Emitted when an item has been double clicked.
     * @param index the index position of the item
     */
    signal itemDoubleClicked(int index)

    /**
     * @brief Emitted when an item has been right clicked. On mobile devices this is translated from a long press and release.
     * @param index the index position of the item
     */
    signal itemRightClicked(int index)

    /**
     * @brief Emitted when an item left emblem badge has been clicked.
     * This is actually a signal to checkable item being toggled.
     * @param index the index position of the item
     */
    signal itemLeftEmblemClicked(int index)


    /**
     * @brief Emitted when an empty area of the browser has been right clicked.
     */
    signal rightClicked()

    /**
     * @brief Emitted when an empty area of the browser has been clicked.
     * @param mouse the object with the event information
     */
    signal areaClicked(var mouse)


    /**
     * @brief A key, physical or not, has been pressed.
     *  @param event the event object contains the relevant information
     */
    signal keyPress(var event)

    /**
     * @brief Emitted when a list of file URLS has been dropped onto the file browser area.
     * @param urls the list of URLs of the files dropped
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
            text: i18nd("mauikitfilebrowsing", "Back")
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
        placeholderText: _filterButton.checked ? i18nd("mauikitfilebrowsing", "Filter") : ("Search")
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
            //            text: i18nd("mauikitfilebrowsing", "Filter")
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
        text: i18nd("mauikitfilebrowsing", "Empty Trash")
        onClicked: FB.FM.emptyTrash()
    }

    Loader
    {
        id: dialogLoader
    }

    Component
    {
        id: _quitSearchDialogComponent

        Maui.InfoDialog
        {
            title: i18n("Quit")
            message: i18n("Are you sure you want to quit the current search in progress?")
            onAccepted:
            {
                _stackView.pop()
                _browser.forceActiveFocus()
            }

            onRejected: close()
        }
    }

    Component
    {
        id: removeDialogComponent

        Maui.FileListingDialog
        {
            id: _removeDialog

            property double freedSpace : calculateFreedSpace(urls)

            title:  i18nd("mauikitfilebrowsing", "Removing %1 files", urls.length)
            message: i18nd("mauikitfilebrowsing", "Delete %1  \nTotal freed space %2", (Maui.Handy.isLinux ? "or move to trash?" : "? This action can not be undone."),  Maui.Handy.formatSize(freedSpace))

            actions: [
                Action
                {
                    text: i18nd("mauikitfilebrowsing", "Cancel")
                    onTriggered: _removeDialog.close()
                },

                Action
                {
                    text: i18nd("mauikitfilebrowsing", "Delete")
                    onTriggered:
                    {
                        control.currentFMList.removeFiles(urls)
                        close()
                    }
                },
                Action
                {
                    text: i18nd("mauikitfilebrowsing", "Trash")
                    enabled: Maui.Handy.isLinux
                    onTriggered:
                    {
                        control.currentFMList.moveToTrash(urls)
                        close()
                    }
                }
            ]

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
        //
        Maui.InputDialog
        {
            id: _newDialog

            title: _newDirOp.checked ? i18nd("mauikitfilebrowsing", "New folder") : i18nd("mauikitfilebrowsing", "New file")
            message: i18nd("mauikitfilebrowsing", "Create a new folder or a file with a custom name.")

            template.iconSource: _newDirOp.checked ? "folder" : FB.FM.getIconName(textEntry.text)
            template.iconVisible: true

            onFinished: (text) =>
            {
                if(_newDirOp.checked)
                {
                    control.currentFMList.createDir(text)
                    return
                }
                if(_newFileOp.checked)
                {
                    control.currentFMList.createFile(text)
                    return
                }
            }

            textEntry.placeholderText: i18nd("mauikitfilebrowsing", "Name")

            Maui.ToolActions
            {
                id: _newActions
                expanded: true
                autoExclusive: true
                display: ToolButton.TextBesideIcon

                Action
                {
                    id: _newDirOp
                    icon.name: "folder-new"
                    text: i18nd("mauikitfilebrowsing", "Folder")
                    checked: String(_newDialog.textEntry.text).indexOf(".") < 0
                }

                Action
                {
                    id: _newFileOp
                    icon.name: "document-new"
                    text: i18nd("mauikitfilebrowsing", "File")
                    checked: String(_newDialog.textEntry.text).indexOf(".") >= 0
                }
            }
        }
    }

    Component
    {
        id: renameDialogComponent

        Maui.InputDialog
        {
            id: _renameDialog

            property var item : ({})

            title: i18nd("mauikitfilebrowsing", "Rename")
            message: i18nd("mauikitfilebrowsing", "Change the name of a file or folder. Write a new name and click Rename to apply the change.")

            //            headBar.visible: false

            template.iconSource: item.icon
            template.imageSource: item.thumbnail
            template.iconSizeHint: Maui.Style.iconSizes.huge

            textEntry.text: item.label
            textEntry.placeholderText: i18nd("mauikitfilebrowsing", "New name")

            onFinished: control.currentFMList.renameFile(item.path, textEntry.text)
            onRejected: close()

            //            acceptButton.text: i18nd("mauikitfilebrowsing", "Rename")
            //            rejectButton.text: i18nd("mauikitfilebrowsing", "Cancel")

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
        }
    }

    Component
    {
        id: _newTagDialogComponent
        FB.NewTagDialog {}
    }

    /**
     * @private
     */
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
                control.openItem(index)
                event.accepted = true
            }

            // Shortcut for pasting an item
            if((event.key == Qt.Key_V) && (event.modifiers & Qt.ControlModifier))
            {
                control.paste()
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
            control.itemClicked(index)
            control.currentView.forceActiveFocus()
        }

        function onItemDoubleClicked(index)
        {
            control.currentIndex = index
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
            if(control.isSearchView)
                return

                if(!Maui.Handy.isMobile && mouse.button === Qt.RightButton)
                {
                    control.rightClicked()
                }

                control.areaClicked(mouse)
                control.currentView.forceActiveFocus()
        }
    }

    Maui.ContextualMenu
    {
        id: _dropMenu
        property string urls
        enabled: !control.isSearchView

        MenuItem
        {
            enabled: !control.readOnly
            text: i18nd("mauikitfilebrowsing", "Copy Here")
            icon.name: "edit-copy"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                console.log("COPY THESE URLS,", urls, _dropMenu.urls)
                control.currentFMList.copyInto(urls)
            }
        }

        MenuItem
        {
            enabled: !control.readOnly
            text: i18nd("mauikitfilebrowsing", "Move Here")
            icon.name: "edit-move"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                control.currentFMList.cutInto(urls)
            }
        }

        MenuItem
        {
            enabled: !control.readOnly

            text: i18nd("mauikitfilebrowsing", "Link Here")
            icon.name: "edit-link"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                for(var i in urls)
                    control.currentFMList.createSymlink(urls[i])
            }
        }

        MenuItem
        {
            enabled: FB.FM.isDir(_dropMenu.urls.split(",")[0])
            text: i18nd("mauikitfilebrowsing", "Open Here")
            icon.name: "folder-open"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                control.browser.path = urls[0]
            }
        }

        MenuSeparator {}

        MenuItem
        {
            text: i18nd("mauikitfilebrowsing", "Cancel")
            icon.name: "dialog-cancel"
            onTriggered: _dropMenu.close()
        }
    }

    StackView
    {
        id: _stackView
        anchors.fill: parent

        initialItem: DropArea
        {
            id: _dropArea

            readonly property alias browser : _browser
            readonly property alias currentFMList : _browser.currentFMList
            readonly property alias currentFMModel: _browser.currentFMModel
            property alias filter: _browser.filter
            property alias filters: _browser.filters
            readonly property alias title : _browser.title

            onDropped: (drop) =>
            {
                if(drop.hasUrls)
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

                Binding on currentIndex
                {
                    value: control.currentIndex
                    restoreMode: Binding.RestoreBindingOrValue
                }

                Loader
                {
                    active: (control.currentPath === "tags://" ||  control.currentPath === "tags:///") && !control.readOnly
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
                readOnly: true
                path: control.currentPath
                Binding on currentIndex
                {
                    value: control.currentIndex
                    restoreMode: Binding.RestoreBindingOrValue
                }

                objectName: "searchView"
                gridItemSize: control.gridItemSize
                listItemSize: control.listItemSize

                currentFMList.autoLoad: false
                settings.viewType: control.settings.viewType
                settings.sortBy: control.settings.sortBy
                settings.showHiddenFiles: control.settings.showHiddenFiles
                settings.group: control.settings.group
                settings.foldersFirst: control.settings.foldersFirst

            }
        }
    }

    Component.onCompleted:
    {
        control.currentView.forceActiveFocus()
    }

    /**
     * @brief Copy the given file URLs to the clipboard
     * @param urls the set of URLs to be copied
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
     * @brief Add the given URLs to the clipboard and mark them as a cut operation
     * @param urls the set of URLs to cut
     **/
    function cut(urls)
    {
        if(control.readOnly)
            return

            if(urls.length <= 0)
            {
                return
            }

            Maui.Handy.copyToClipboard({"urls": urls}, true)
    }

    /**
     * Paste the contents of the clipboard into the current location, if supported.
     **/
    function paste()
    {
        control.currentFMList.paste()
    }

    /**
     * Remove the given URLs.Array()this will launch a dialog to confirm this action.
     * @param urls the set of URLs to be removed
     **/
    function remove(urls)
    {
        if(urls.length <= 0)
        {
            return
        }

        dialogLoader.sourceComponent = removeDialogComponent
        dialog.urls = urls
        dialog.open()
    }

    /**
     * @brief Given an index position of a element, try to open it, it can be a directory, a file or an executable.
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
     * @brief Open a file of the given path URL in the dedicated application
     * @param path The URL of the file
     **/
    function openFile(path)
    {
        FB.FM.openUrl(path)
    }

    /**
     * @brief Open a folder location
     * @param path the URL of the folder location
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
     * @brief Go back to the previous location
     **/
    function goBack()
    {
        openFolder(control.currentFMList.previousPath())
    }

    /**
     * @brief Go forward to the location before going back
     **/
    function goForward()
    {
        openFolder(control.currentFMList.posteriorPath())
    }

    /**
     * @brief Go to the location parent directory
     **/
    function goUp()
    {
        openFolder(control.currentFMList.parentPath)
    }

    /**
     * @brief Add an item to the selection
     * @param item the item object/map representing the file to be added to the selection
     * @warning For this to work the implementation needs to have passed a `selectionBar`
     * @see selectionBar
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
     * @brief Given a list of indexes add them to the selection
     * @param indexes list of index positions
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
     * @brief Forces to select all the entries
     *
     * @bug If there are too many entries, this could freeze the UI
     **/
    function selectAll() //TODO for now dont select more than 100 items so things dont freeze or break
    {
        if(control.selectionBar == null)
        {
            return
        }

        selectIndexes([...Array( control.currentFMList.count ).keys()])
    }

    /**
     * @brief Add a bookmark to a given list of paths
     * @param paths a group of directory URLs to be bookmarked
     **/
    function bookmarkFolder(paths) //multiple paths
    {
        for(var i in paths)
        {
            FB.FM.bookmark(paths[i])
        }
    }

    /**
     * @brief Open/close the search bar
     */
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
     * @brief Forces to open the search view and start a search.
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
     * @brief Forces to close the search view, and return to the browsing view.
     **/
    function quitSearch()
    {
        if(control.currentView.loading)
        {
            dialogLoader.sourceComponent = _quitSearchDialogComponent
            control.dialog.open()
            return
        }

        _stackView.pop()
        _browser.forceActiveFocus()
    }

    /**
     * @brief Perform a recursive search starting form the current location and down to all the children directories.
     * @param query the text query to match against
     **/
    function search(query)
    {
        openSearch()
        _searchField.text = query

        _stackView.currentItem.title = i18nd("mauikitfilebrowsing", "Search: %1", query)
        _stackView.currentItem.currentFMList.search(query, true)

        _stackView.currentItem.forceActiveFocus()
    }

    /**
     * @brief Opens a dialog for typing the name of the new item.
     * The new item can be a file or directory.
     **/
    function newItem()
    {
        if(control.isSearchView)
            return;

        dialogLoader.sourceComponent = newDialogComponent
        dialog.open()
        dialog.forceActiveFocus()
    }

    /**
     * @brief Opens a dialog for typing the new name of the item
     * This will target the current selected item in the browser view
     **/
    function renameItem()
    {
        if(control.isSearchView)
            return;

        dialogLoader.sourceComponent= renameDialogComponent
        dialog.open()
        dialog.forceActiveFocus()
    }

    /**
     * @brief Opens a dialog to confirm this operation
     * This will target the current selected item in the browser view
     **/
    function removeItem()
    {
        if(control.isSearchView)
            return;

        dialogLoader.sourceComponent= renameDialogComponent
        dialog.open()
        dialog.forceActiveFocus()
    }

    /**
     * @brief Filters the contents of the selection to the current path.
     * @note Keep in mind that the selection bar can have entries from multiple different locations. With this method only the entries which are inside the `currentPath` will be returned.
     *
     * @param currentPath The currentPath must be a directory, so the selection entries can be compared as its parent directory.
     * @param itemPath The itemPath is a default item path in case the selectionBar is empty
     *
     * @return the list of entries in the selection that match the currentPath as their parent directory
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

    /**
     * @brief Forces to focus the current view.
     */
    function forceActiveFocus()
    {
        control.currentView.forceActiveFocus()
    }
}
