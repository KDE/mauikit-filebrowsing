import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import org.mauikit.controls as Maui
import org.mauikit.filebrowsing as FB

/**
 * @inherit org::mauikit::controls::AltBrowser
 * @brief The browsing view implementation for the FileBrowser control.
 *
 * @warning This control is private and only exposed for tweaking its properties. It can not be instantiated manually.
 *
 */
Maui.AltBrowser
{
    id: control

    headBar.visible: false

    title: currentFMList.pathName

    enableLassoSelection: true

    gridView.itemSize : control.gridItemSize
    gridView.itemHeight: gridView.cellWidth

    background: null
    /**
     * @brief It is possible to insert an arbitrary element for each entry. The component here declared will be drawn on top of the entry delegate in the grid view, and on the far right side in the list view.
     * The entry element model details are accessible via the `model` property. For example `model.name`, `model.url`, etc.
     */
    property Component delegateInjector : null

    /**
     * @brief An alias to the current popup dialog being shown.
     * @property Item BrowserView::dialog
     */
    readonly property alias dialog :_dialogLoader.item
    Loader
    {
        id: _dialogLoader
    }

    QtObject
    {
        id: _private
        property int gridIconSize: Maui.Style.mapToIconSizes(control.gridItemSize)
        property int listIconSize:  Maui.Style.mapToIconSizes(control.listItemSize)
    }

    Binding on currentIndex
    {
        when: control.currentView
        value: control.currentView.currentIndex
    }

    viewType: settings.viewType === FB.FMList.ICON_VIEW ? Maui.AltBrowser.ViewType.Grid : Maui.AltBrowser.ViewType.List

    onPathChanged:
    {
        control.currentIndex = 0
        control.currentView.forceActiveFocus()
    }

    model: Maui.BaseModel
    {
        id: _browserModel
        list: FB.FMList
        {
            id: _commonFMList
            path: control.path
            onSortByChanged: if(settings.group) groupBy()
            onlyDirs: settings.onlyDirs
            filterType: settings.filterType
            filters: settings.filters
            sortBy: settings.sortBy
            hidden: settings.showHiddenFiles
            foldersFirst: settings.foldersFirst
        }

        recursiveFilteringEnabled: true
        sortCaseSensitivity: Qt.CaseInsensitive
        filterCaseSensitivity: Qt.CaseInsensitive
    }

    /**
     * @brief The current location path.
     * @see FMList::path
     * @property string BrowserView::path
     */
    property alias path : _commonFMList.path

    /**
     * @brief The size of the items in the grid. This is the total sum of the thumbnail icon and the name label.
     * The icon size is calculated to match always a standard icon size.
     * By default this is set to `140`
     */
    property int gridItemSize : 140

    /**
     * @brief The height size of the list elements.
     * By default this is set to `Style.rowHeight`
     */
    property int listItemSize : Maui.Style.rowHeight

    /**
     * @brief An alias to access the grouped setting preferences for tweaking the file listing properties.
     * @see BrowserSettings
     * @property BrowserSettings BrowserView::settings
     */
    readonly property alias settings : _settings

    /**
     * @brief Whether the listing of the location contents is still loading. This can be false if the contents are ready or have failed, to check those other conditions refer to the FMList::status property.
     * @see FMList::status::code
     */
    readonly property bool loading : currentFMList.status.code === FB.PathStatus.LOADING

    /**
     * @see FMList::readOnly
     * @property bool BrowserView::readOnly
     */
    property alias readOnly: _commonFMList.readOnly

    /**
     * @brief An alias to the FMList model list and controller for listing the location contents and exposing the browsing management action methods.
     * @note The sorting of the location contents is done via this object properties, and not using the MauiKit MauiModel wrapper.
     * @see currentFMModel
     *
     * @property FMList BrowserView::currentFMList
     */
    readonly property alias currentFMList : _commonFMList

    /**
     * @brief An alias to the MauiKit MauiModel, wrapping the currentFMList.
     * @property MauiKit::MauiModel BrowserView::currentFMModel
     */
    readonly property alias currentFMModel : _browserModel

    /**
     * @brief The string value to filter the location contents.
     * @see MauiKit::MauiModel::filter BrowserView::filter
     */
    property alias filter : _browserModel.filter

    /**
     * @brief The list of strings values to filter the location contents.
     * @see MauiKit::MauiModel::filter BrowserView::filters
     */
    property alias filters: _browserModel.filters

    /**
     * @brief Emitted when an entry item has been clicked.
     * @param index the index position of the item
     * @note To correctly map and index position to the actual item entry in the model use the `currentFMModel.get()` method, this will take care of correctly mapping the indexes in case the content has been filtered or sorted.
     */
    signal itemClicked(int index)

    /**
     * @brief Emitted when an entry has been double clicked.
     * @param index the index position of the item
     */
    signal itemDoubleClicked(int index)

    /**
     * @brief Emitted when an entry has been right clicked.
     * @param index the index position of the item
     */
    signal itemRightClicked(int index)

    /**
     * @brief Emitted when an entry selection has been toggled.
     * @param index the index position of the item
     * @param state the checked state
     */
    signal itemToggled(int index, bool state)

    /**
     * @brief Emitted when a set of entries has been selected by using the lasso selection.
     * @param indexes the list of indexes positions selected
     */
    signal itemsSelected(var indexes)

    /**
     * @brief Emitted when a keyboard key has been pressed
     * @param event the object with the event information
     */
    signal keyPress(var event)

    /**
     * @brief Emitted when the background area has been clicked
     * @param mouse the object with the event information
     */
    signal areaClicked(var mouse)

    /**
     * @brief Emitted when the background area has been right clicked. This can be consumed for launching a contextual menu.
     * @param mouse the object with the event information
     */
    signal areaRightClicked(var mouse)

    Connections
    {
        target: control.currentView
        ignoreUnknownSignals: true

        function onKeyPress(event)
        {
            control.keyPress(event)
        }

        function onItemsSelected(indexes)
        {
            control.itemsSelected(indexes)
        }

        function onAreaClicked(mouse)
        {
            console.log("Area clicked")
            control.currentView.forceActiveFocus()
            control.areaClicked(mouse)
        }

        function onAreRightClicked(mouse)
        {
            console.log("Area right clicked")

            control.currentView.forceActiveFocus()
            control.areaRightClicked(mouse)
        }
    }

    BrowserSettings
    {
        id: _settings
        onGroupChanged:
        {
            if(settings.group)
            {
                groupBy()
            }
            else
            {
                currentView.section.property = ""
            }
        }
    }

    BrowserHolder
    {
        id: _holder
        browser: _commonFMList
    }

    Maui.ProgressIndicator
    {
        id: _scanningProgress
        width: parent.width
        anchors.bottom: parent.bottom
        visible: control.loading
    }

    holder.visible: _holder.visible
    holder.emoji: _holder.emoji
    holder.title: _holder.title
    holder.body: _holder.body

    Maui.ContextualMenu
    {
        id: _dropMenu
        property string urls
        property url target

        MenuItem
        {
            Component
            {
                id: _mergeDialogComponent

                Maui.InputDialog
                {
                    id: _mergeDialog
                    property var urls

                    readonly property bool dirExists : FB.FM.fileExists(control.path+"/"+textEntry.text)

                    onDirExistsChanged:
                    {
                        console.log("DIR EXISTS?", dirExists)

                        if(dirExists)
                        {
                            _mergeDialog.alert(i18nd("mauikitfilebrowsing", "Directory already exists."), 2)
                        }else
                        {
                            _mergeDialog.alert(i18nd("mauikitfilebrowsing", "Looks good."), 0)
                        }
                    }

                    title: i18nd("mauikitfilebrowsing", "Merge %1 files", urls.length)
                    message:i18nd("mauikitfilebrowsing", "Give a name to the new directory where all files will be merge.")

                    textEntry.placeholderText: i18nd("mauikitfilebrowsing", "Directory name")

                    onFinished:
                    {
                        FB.FM.group(_mergeDialog.urls, control.path, text)
                    }
                }
            }

            enabled: !FB.FM.isDir(_dropMenu.target) && !control.readOnly
            text: i18nd("mauikitfilebrowsing", "Merge Here")
            icon.name: "edit-group"
            onTriggered:
            {
                var urls = _dropMenu.urls.split(",")
                urls.push(_dropMenu.target)
                _dialogLoader.sourceComponent = _mergeDialogComponent
                dialog.urls = urls
                dialog.open()
            }
        }

        MenuItem
        {
            enabled: FB.FM.isDir(_dropMenu.target) && !control.readOnly
            text: i18nd("mauikitfilebrowsing", "Copy Here")
            icon.name: "edit-copy"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                FB.FM.copy(urls, _dropMenu.target)
            }
        }

        MenuItem
        {
            enabled: FB.FM.isDir(_dropMenu.target) && !control.readOnly
            text: i18nd("mauikitfilebrowsing", "Move Here")
            icon.name: "edit-move"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                FB.FM.cut(urls, _dropMenu.target)
            }
        }

        MenuItem
        {
            enabled: FB.FM.isDir(_dropMenu.target) && !control.readOnly
            text: i18nd("mauikitfilebrowsing", "Link Here")
            icon.name: "edit-link"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                for(var i in urls)
                    FB.FM.createSymlink(url[i], _dropMenu.target)
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

    listView.section.delegate: Maui.LabelDelegate
    {
        id: delegate
        width: ListView.view.width
        height: Maui.Style.toolBarHeightAlt
        text: control.listView.section.property == "date" || control.listView.section.property === "modified" ?  Qt.formatDateTime(new Date(section), "d MMM yyyy") : section
        isSection: true
    }

    listDelegate: Maui.ListBrowserDelegate
    {
        id: delegate
        readonly property string path : model.path

        width: ListView.view.width

        iconSource: model.icon

        label1.text: model.label ? model.label : ""
        label2.text: control.objectName === "searchView" ? model.path : ""
        label3.text : model.mime ? (model.mime === "inode/directory" ? (model.count ? model.count + i18nd("mauikitfilebrowsing", " items") : "") : Maui.Handy.formatSize(model.size)) : ""
        label4.text: model.modified ? Maui.Handy.formatDate(model.modified, "MM/dd/yyyy") : ""

        template.isMask: iconSizeHint <= 22
        iconSizeHint: _private.listIconSize

        tooltipText: model.path

        checkable: control.selectionMode || checked
        imageSource: settings.showThumbnails && height > 32 ? model.thumbnail : ""
        checked: selectionBar ? selectionBar.contains(model.path) : false
        template.iconContainer.opacity: model.hidden == "true" ? 0.5 : 1
        draggable: true

        Drag.keys: ["text/uri-list"]
        Drag.mimeData: {
            "text/uri-list": filterSelection(control.path, model.path)
        } 

        Item
        {
            Layout.fillHeight: true
            Layout.preferredWidth: height
            visible: (model.issymlink == true) || (model.issymlink == "true")

            Maui.Icon
            {
                source: "link"
                height: Maui.Style.iconSizes.small
                width: Maui.Style.iconSizes.small
                anchors.centerIn: parent
                isMask: true
                color: label1.color
            }
        }

        Loader
        {
            id: _injectorLoader
            asynchronous: true
            property var itemData : model
            sourceComponent: control.delegateInjector
            Layout.fillHeight: true
            Layout.preferredWidth: visible ? height : 0
            active: control.delegateInjector
            visible: active
        }

        onClicked: (mouse) =>
        {
            control.currentIndex = index

            if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier))
            {
                control.itemsSelected([index])
            }else if((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ShiftModifier))
            {
                var lastSelectedIndex = findLastSelectedIndex(control.listView.flickable, index)

                if(lastSelectedIndex < 0)
                {
                    return
                }
                control.itemsSelected(control.range(lastSelectedIndex, index))

            }else
            {
                control.itemClicked(index)
            }
        }

        onDoubleClicked:
        {
            control.currentIndex = index
            control.itemDoubleClicked(index)
        }

        onPressAndHold:
        {
            if(!Maui.Handy.isTouch)
                return

                control.currentIndex = index
                control.itemRightClicked(index)
        }

        onRightClicked:
        {
            control.currentIndex = index
            control.itemRightClicked(index)
        }

        onToggled: (state) =>
        {
            control.currentIndex = index
            control.itemToggled(index, state)
        }

        onContentDropped: (drop) =>
        {
            _dropMenu.urls = drop.urls.join(",")
            _dropMenu.target = model.path
            _dropMenu.show()
        }

        ListView.onRemove:
        {
            if(selectionBar)
            {
                selectionBar.removeAtUri(delegate.path)
            }
        }

        Connections
        {
            target: selectionBar

            function onUriRemoved(uri)
            {
                if(uri === model.path)
                    delegate.checked = false
            }

            function onUriAdded(uri)
            {
                if(uri === model.path)
                    delegate.checked = true
            }

            function onCleared()
            {
                delegate.checked = false
            }
        }
    }

    gridDelegate: Item
    {
        height: GridView.view.cellHeight
        width: GridView.view.cellWidth
        readonly property alias checked : delegate.checked

        GridView.onRemove:
        {
            if(selectionBar)
            {
                selectionBar.removeAtUri(delegate.path)
            }
        }

        Maui.GridBrowserDelegate
        {
            id: delegate
            readonly property string path : model.path

            template.imageWidth: control.gridView.itemSize
            template.imageHeight: control.gridView.itemSize

            anchors.fill: parent
            anchors.margins: Maui.Handy.isMobile ? Maui.Style.space.tiny : Maui.Style.space.medium

            template.labelSizeHint: 42
            iconSizeHint: _private.gridIconSize
            imageSource: settings.showThumbnails ? model.thumbnail : ""
            template.fillMode: Image.PreserveAspectFit
            template.maskRadius: 0
            iconSource: model.icon
            label1.text: model.label
            label2.visible: delegate.height > 160 && model.mime
            label2.font.pointSize: Maui.Style.fontSizes.tiny
            label2.text: model.mime ? (model.mime === "inode/directory" ? (model.count ? model.count + i18nd("mauikitfilebrowsing", " items") : "") : Maui.Handy.formatSize(model.size)) : ""

            isCurrentItem: parent.GridView.isCurrentItem || checked
            tooltipText: model.label
            checkable: control.selectionMode || checked
            checked: (selectionBar ? selectionBar.contains(model.path) : false)
            draggable: true
            template.iconContainer.opacity: model.hidden == "true" ? 0.5 : 1

            Drag.keys: ["text/uri-list"]
            Drag.mimeData: {
                "text/uri-list":  filterSelection(control.path, model.path)
            } 

            Maui.Icon
            {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: Maui.Style.space.medium

                visible: (model.issymlink == true) || (model.issymlink == "true")

                source: "link"
                color: Maui.Theme.textColor
                isMask: true
                height: Maui.Style.iconSizes.small
            }

            Loader
            {
                id: _injectorLoader
                asynchronous: true
                property var itemData : model
                sourceComponent: control.delegateInjector
                anchors.fill: parent
                active: control.delegateInjector
            }

            onClicked: (mouse) =>
            {
                if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier))
                {
                    control.itemsSelected([index])
                }else if((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ShiftModifier))
                {
                    var lastSelectedIndex = findLastSelectedIndex(control.gridView.flickable, index)

                    if(lastSelectedIndex < 0)
                    {
                        return
                    }
                    control.itemsSelected(control.range(lastSelectedIndex, index))

                }else
                {
                    control.itemClicked(index)
                }
                control.currentIndex = index
            }

            onDoubleClicked:
            {
                control.currentIndex = index
                control.itemDoubleClicked(index)
            }

            onPressAndHold:
            {
                if(!Maui.Handy.isTouch)
                    return

                    control.currentIndex = index
                    control.itemRightClicked(index)
            }

            onRightClicked:
            {
                control.currentIndex = index
                control.itemRightClicked(index)
            }

            onToggled: (state) =>
            {
                control.currentIndex = index

                control.itemToggled(index, state)
            }

            onContentDropped: (drop) =>
            {
                _dropMenu.urls = drop.urls.join(",")
                _dropMenu.target = model.path
                _dropMenu.show()
            }

            Connections
            {
                target: selectionBar

                function onUriRemoved(uri)
                {
                    if(uri === model.path)
                        delegate.checked = false
                }

                function onUriAdded(uri)
                {
                    if(uri === model.path)
                        delegate.checked = true
                }

                function onCleared(uri)
                {
                    delegate.checked = false
                }
            }
        }
    }

    /**
     * @brief Forces the view to re-organize the content entries into subgroups, that will depend on the current sorting key o group the entries by name, size, or date, etc.
     * @note When this is invoked the view will go into the list mode - grouping is not supported in the grid view mode.
     */
    function groupBy()
    {
        var prop = ""
        var criteria = ViewSection.FullString

        switch(control.currentFMList.sortBy)
        {
            case FB.FMList.LABEL:
                prop = "label"
                criteria = ViewSection.FirstCharacter
                break;
            case FB.FMList.MIME:
                prop = "mime"
                break;
            case FB.FMList.SIZE:
                prop = "size"
                break;
            case FB.FMList.DATE:
                prop = "date"
                break;
            case FB.FMList.MODIFIED:
                prop = "modified"
                break;
        }

        if(!prop)
        {
            control.currentView.section.property = ""
            return
        }

        control.settings.viewType = FB.FMList.LIST_VIEW
        control.currentView.section.property = prop
        control.currentView.section.criteria = criteria
    }

    /**
     * @private
     */
    function findLastSelectedIndex(view, limit)
    {
        var res = -1;
        for(var i = 0; i < limit; i++)
        {
            if(view.itemAtIndex(i).checked)
            {
                res = i
            }
        }

        return res;
    }

    /**
     * @private
     */
    function range(start, end)
    {
        const isReverse = (start > end);
        const targetLength = isReverse ? (start - end) + 1 : (end - start ) + 1;
        const arr = new Array(targetLength);
        const b = Array.apply(null, arr);
        const result = b.map((discard, n) => {
            return (isReverse) ? n + end : n + start;
        });

        return (isReverse) ? result.reverse() : result;
    }

    /**
     * @brief Forces to focus the current browsing view first element.
     */
    function forceActiveFocus()
    {
        control.currentView.forceActiveFocus()
    }
}
