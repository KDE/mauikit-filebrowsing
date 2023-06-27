import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.3

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.3 as FB

Maui.AltBrowser
{
    id: control
    
    headBar.visible: false
    
    title: currentFMList.pathName
    selectionMode: control.selectionMode
    enableLassoSelection: true
    
    gridView.itemSize : control.gridItemSize
    gridView.itemHeight: gridView.cellWidth
    //     gridView.cacheBuffer: control.height * 10
    
    property Component delegateInjector : null
    
    property alias dialog :_dialogLoader.item
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
     * 
     */
    property alias path : _commonFMList.path
    
    /**
     * 
     */
    
    property int gridItemSize : 140
    property int listItemSize : Maui.Style.rowHeight
    
    /**
     * 
     */
    property alias settings : _settings
    
    readonly property bool loading : currentFMList.status.code === FB.PathStatus.LOADING
    
    property alias readOnly: _commonFMList.readOnly
    /**
     * 
     */
    readonly property alias currentFMList : _commonFMList
    
    /**
     * 
     */
    readonly property alias currentFMModel : _browserModel    
    
    /**
     * 
     */
    property alias filter : _browserModel.filter
    
    property alias filters: _browserModel.filters
    
    signal itemClicked(int index)
    signal itemDoubleClicked(int index)
    signal itemRightClicked(int index)
    signal itemToggled(int index, bool state)
    signal itemsSelected(var indexes)
    signal keyPress(var event)
    signal areaClicked(var mouse)
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
                
                Maui.NewDialog
                {
                    id: _mergeDialog
                    property var urls
                    
                    headBar.visible: false
                    
                    readonly property bool dirExists : FB.FM.fileExists(control.path+"/"+textEntry.text)
                    acceptButton.enabled: !dirExists
                    
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
            text: i18nd("mauikitfilebrowsing", "Merge here")
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
            text: i18nd("mauikitfilebrowsing", "Copy here")
            icon.name: "edit-copy"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                FB.FM.copy(urls, _dropMenu.target, false)
            }
        }
        
        MenuItem
        {
            enabled: FB.FM.isDir(_dropMenu.target) && !control.readOnly
            text: i18nd("mauikitfilebrowsing", "Move here")
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
            text: i18nd("mauikitfilebrowsing", "Link here")
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
        label: control.listView.section.property == "date" || control.listView.section.property === "modified" ?  Qt.formatDateTime(new Date(section), "d MMM yyyy") : section        
        isSection: true
    }

    listDelegate: Maui.ListBrowserDelegate
    {
        id: delegate
        readonly property string path : model.path

        width: ListView.view.width
//         template.headerSizeHint: control.listItemSize
        iconSource: model.icon

        label1.text: model.label ? model.label : ""
        label2.text: control.objectName === "searchView" ? model.path : ""
        label3.text : model.mime ? (model.mime === "inode/directory" ? (model.count ? model.count + i18nd("mauikitfilebrowsing", " items") : "") : Maui.Handy.formatSize(model.size)) : ""
        label4.text: model.modified ? Maui.Handy.formatDate(model.modified, "MM/dd/yyyy") : ""

        template.isMask: iconSizeHint <= 22
        iconSizeHint: _private.listIconSize

        tooltipText: model.path

        checkable: control.selectionMode
        imageSource: settings.showThumbnails && height > 32 ? model.thumbnail : ""
        checked: selectionBar ? selectionBar.contains(model.path) : false
        template.iconContainer.opacity: model.hidden == "true" ? 0.5 : 1
        draggable: true

        Drag.keys: ["text/uri-list"]
        Drag.mimeData: Drag.active ?
        {
            "text/uri-list": filterSelection(control.path, model.path).join("\n")
        } : {}

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

        onClicked:
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

        onToggled:
        {
            control.currentIndex = index
            control.itemToggled(index, state)
        }

        onContentDropped:
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
            checkable: control.selectionMode
            checked: (selectionBar ? selectionBar.contains(model.path) : false)
            draggable: true
            template.iconContainer.opacity: model.hidden == "true" ? 0.5 : 1

            Drag.keys: ["text/uri-list"]
            Drag.mimeData: Drag.active ?
            {
                "text/uri-list":  filterSelection(control.path, model.path).join("\n")
            } : {}

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

            onClicked:
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

            onToggled:
            {
                control.currentIndex = index
                control.itemToggled(index, state)
            }

            onContentDropped:
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
     * 
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
    
    function forceActiveFocus()
    {
        control.currentView.forceActiveFocus()        
    }
}
