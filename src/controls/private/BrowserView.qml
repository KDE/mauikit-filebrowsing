import QtQuick 2.9
import QtQuick.Controls 2.9
import QtQuick.Layouts 1.3

import org.kde.kirigami 2.7 as Kirigami

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
    gridView.itemHeight: Math.floor(control.gridView.itemSize*1.4)
    //     gridView.cacheBuffer: control.height * 10
    
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
        
        filter: control.filter
        recursiveFilteringEnabled: true
        sortCaseSensitivity: Qt.CaseInsensitive
        filterCaseSensitivity: Qt.CaseInsensitive
    }
    
    /**
     * 
     */
    property url path
    
    /**
     * 
     */
    
    property int gridItemSize :  Maui.Style.iconSizes.large * 1.7
    property int listItemSize : Maui.Style.rowHeight
    
    /**
     * 
     */
    property alias settings : _settings
    
    
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
    property string filter
    
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
        visible: _commonFMList.status.code === FB.PathStatus.LOADING
    }
    
    holder.visible: _holder.visible
    holder.emoji: _holder.emoji
    holder.title: _holder.title
    holder.body: _holder.body
    holder.emojiSize: _holder.emojiSize
    
    Maui.ContextualMenu
    {
        id: _dropMenu
        property string urls
        property url target
        
        MenuItem
        {            
            Maui.NewDialog
            {
                id: _mergeDialog
                property var urls
                readonly property bool dirExists : FB.FM.fileExists(control.path.toString()+"/"+textEntry.text)
                acceptButton.enabled: !dirExists
                
                onDirExistsChanged:
                {
                    if(dirExists)
                    {
                        _mergeDialog.alert(i18n("Directory already exists"), 2)
                    }
                }
                
                title: i18n("Merge %1 files", urls.length)
                message:i18n("Give a name to the new directory where all files will be merge.")
                
                textEntry.placeholderText: i18n("Directory name")
                
                onFinished:
                {                    
                    FB.FM.group(_mergeDialog.urls, control.path, text)                    
                }
            }
            
            enabled: !FB.FM.isDir(_dropMenu.target)
            text: i18n("Merge here")
            icon.name: "edit-group"
            onTriggered:
            {
                var urls = _dropMenu.urls.split(",")
                urls.push(_dropMenu.target)
                _mergeDialog.urls = urls
                _mergeDialog.open()
            }
        }        
        
        MenuItem
        {
            enabled: FB.FM.isDir(_dropMenu.target) 
            text: i18n("Copy here")
            icon.name: "edit-copy"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                FB.FM.copy(urls, _dropMenu.target, false)
            }
        }
        
        MenuItem
        {
            enabled: FB.FM.isDir(_dropMenu.target)
            text: i18n("Move here")
            icon.name: "edit-move"
            onTriggered:
            {
                const urls = _dropMenu.urls.split(",")
                FB.FM.cut(urls, _dropMenu.target)
            }
        }
        
        MenuItem
        {
            enabled: FB.FM.isDir(_dropMenu.target)
            text: i18n("Link here")
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
            text: i18n("Cancel")
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
        labelTxt.font.pointSize: Maui.Style.fontSizes.big
        
        isSection: true
    }
    
    listDelegate: Maui.ListBrowserDelegate
    {
        id: delegate
        readonly property string path : model.path
        
        width: ListView.view.width
        height: control.listItemSize
        
        iconSource: model.icon
        
        label1.text: model.label ? model.label : ""
        label3.text : model.mime ? (model.mime === "inode/directory" ? (model.count ? model.count + i18n(" items") : "") : Maui.Handy.formatSize(model.size)) : ""
        label4.text: model.modified ? Maui.Handy.formatDate(model.modified, "MM/dd/yyyy") : ""
        
        iconSizeHint : Maui.Style.iconSizes.medium
        
        tooltipText: model.path
        
        checkable: control.selectionMode
        imageSource: settings.showThumbnails && height > 64 ? model.thumbnail : ""
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
            
            Kirigami.Icon
            {
                source: "link"
                height: Maui.Style.iconSizes.small
                width: Maui.Style.iconSizes.small
                anchors.centerIn: parent
                isMask: true
                color: label1.color
            }
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
            anchors.margins: Kirigami.Settings.isMobile ? Maui.Style.space.tiny : Maui.Style.space.medium 
            
            template.labelSizeHint: 42
            iconSizeHint: Maui.Style.mapToIconSizes(template.iconContainer.height)
            imageSource: settings.showThumbnails ? model.thumbnail : ""
            template.fillMode: Image.PreserveAspectFit
            iconSource: model.icon
            label1.text: model.label
            //             label2.visible: delegate.height > 160 && model.mime
            //             label2.font.pointSize: Maui.Style.fontSizes.tiny
            //             label2.text: model.mime ? (model.mime === "inode/directory" ? (model.count ? model.count + i18n(" items") : "") : Maui.Handy.formatSize(model.size)) : ""
            
            padding: Maui.Style.space.tiny
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
            
            Kirigami.Icon
            {
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: Maui.Style.space.medium
                
                visible: (model.issymlink == true) || (model.issymlink == "true")
                
                source: "link"
                color: Kirigami.Theme.textColor
                isMask: true
                height: Maui.Style.iconSizes.small
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
}
