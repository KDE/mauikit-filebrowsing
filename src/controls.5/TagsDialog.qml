import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3

import org.mauikit.controls 1.3 as Maui
import org.mauikit.filebrowsing 1.3 as FB

/**
 * TagsDialog
 * A global sidebar for the application window that can be collapsed.
 *
 *
 *
 *
 *
 *
 */
Maui.PopupPage
{
    id: control
    
    /**
     * taglist : TagsList
     */
    property alias taglist : _tagsList
    
    /**
     * listView : ListView
     */
    property alias listView: _listView
    
    /**
     * composerList : TagsList
     */
    property alias composerList: tagListComposer.list
    
    /**
     * tagsReady :
     */
    signal tagsReady(var tags)
    
    closeButtonVisible: false
    
    hint: 1
    
    maxHeight: 500
    maxWidth: 400
    
    actions: [
        
        Action
        {
            text: i18nd("mauikitfilebrowsing", "Save")
            onTriggered: setTags()
        },
        
        Action
        {
            text: i18nd("mauikitfilebrowsing", "Cancel")
            onTriggered: control.close()
        }
    ]
    
    headBar.visible: true
    headBar.forceCenterMiddleContent: false
    headBar.middleContent: TextField
    {
        id: tagText
        Layout.fillWidth: true
        Layout.maximumWidth: 500
        placeholderText: i18nd("mauikitfilebrowsing", "Filter or add a new tag")
        //             validator: RegExpValidator { regExp: /[0-9A-F]+/ }
        onAccepted:
        {
            const tags = tagText.text.split(",")
            for(var i in tags)
            {
                const myTag = tags[i].trim()
                _tagsList.insert(myTag)
                tagListComposer.list.append(myTag)
            }
            clear()
            _tagsModel.filter = ""
        }
        
        onTextChanged:
        {
            _tagsModel.filter = text
        }
    }
    
    Maui.InfoDialog
    {
        id: _deleteDialog
        
        property string tag
        title: i18nd("mauikitfilebrowsing", "Delete %1", tag)
        message: i18nd("mauikitfilebrowsing", "Are you sure you want to delete this tag? This action can not be undone.")
        template.iconSource: "tag"
        onAccepted:
        {
            FB.Tagging.removeTag(tag)
            _deleteDialog.close()
        }
        
        onRejected: _deleteDialog.close()
    }
    
    Maui.ContextualMenu
    {
        id: _menu
        
        MenuItem
        {
            text: i18nd("mauikitfilebrowsing", "Edit")
            icon.name: "document-edit"
        }
        
        MenuItem
        {
            text: i18nd("mauikitfilebrowsing", "Delete")
            icon.name: "delete"
            onTriggered:
            {
                _deleteDialog.tag = _tagsModel.get(_listView.currentIndex).tag
                _deleteDialog.open()
            }
        }
    }
    
    stack: [
        
        Maui.ListBrowser
        {
            id: _listView
            
            Layout.fillHeight: true
            Layout.fillWidth: true
            clip: true
            currentIndex: -1
            
            holder.emoji: "qrc:/assets/tag.svg"
            holder.visible: _listView.count === 0
            holder.title : i18nd("mauikitfilebrowsing", "No Tags!")
            holder.body: i18nd("mauikitfilebrowsing", "Create new tags to organize your files.")
            
            model: Maui.BaseModel
            {
                id: _tagsModel
                sort: "tag"
                sortOrder: Qt.AscendingOrder
                recursiveFilteringEnabled: true
                sortCaseSensitivity: Qt.CaseInsensitive
                filterCaseSensitivity: Qt.CaseInsensitive
                list: FB.TagsListModel
                {
                    id: _tagsList
                    strict: false
                }
            }
            
            delegate: Maui.ListBrowserDelegate
            {
                width: ListView.view.width
                label1.text: model.tag
                iconSource: model.icon
                iconSizeHint: Maui.Style.iconSizes.small
                
                template.content: Rectangle
                {
                    color: model.color ? model.color : "transparent"
                    height: Maui.Style.iconSizes.small
                    radius: height/2
                    width: height
                }
                
                onClicked:
                {
                    _listView.currentIndex = index
                    if(Qt.styleHints.singleClickActivation)
                    {
                        tagListComposer.list.appendItem(_tagsModel.get(_listView.currentIndex))
                    }
                }
                
                onDoubleClicked:
                {
                    _listView.currentIndex = index
                    if(!Qt.styleHints.singleClickActivation)
                    {
                        tagListComposer.list.appendItem(_tagsModel.get(_listView.currentIndex))
                    }
                }
                
                onPressAndHold:
                {
                    _listView.currentIndex = index
                    _menu.open()
                }
                
                onRightClicked:
                {
                    _listView.currentIndex = index
                    _menu.open()
                }
            }
        },
        
        Loader
        {
            asynchronous: true
            active: tagListComposer.list.urls.length > 1
            visible: active
            Layout.fillWidth: true
            
            sourceComponent: Maui.ListItemTemplate
            {
                id: _info
                // padding: _listView.padding
                // implicitHeight: Maui.Style.toolBarHeight + Maui.Style.space.huge
                
                property var itemInfo : FB.FM.getFileInfo( tagListComposer.list.urls[0])
                label1.text: i18nd("mauikitfilebrowsing", "Tagging %1 files", tagListComposer.list.urls.length)
                label2.text: i18nd("mauikitfilebrowsing", "Add new tags for the selected files.")
                label2.wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                iconSource : itemInfo.icon
                imageSource: itemInfo.thumbnail
                // iconSizeHint: Maui.Style.iconSizes.huge
                // headerSizeHint: iconSizeHint + Maui.Style.space.big
                
                iconComponent: Item
                {
                    Item
                    {
                        anchors.fill: parent
                        layer.enabled: true
                        
                        Rectangle
                        {
                            anchors.fill: parent
                            anchors.leftMargin: Maui.Style.space.small
                            anchors.rightMargin: Maui.Style.space.small
                            radius: Maui.Style.radiusV
                            color: Qt.tint(control.Maui.Theme.textColor, Qt.rgba(control.Maui.Theme.backgroundColor.r, control.Maui.Theme.backgroundColor.g, control.Maui.Theme.backgroundColor.b, 0.9))
                            border.color: Maui.Theme.backgroundColor
                        }
                        
                        Rectangle
                        {
                            anchors.fill: parent
                            anchors.topMargin: Maui.Style.space.tiny
                            anchors.leftMargin: Maui.Style.space.tiny
                            anchors.rightMargin: Maui.Style.space.tiny
                            radius: Maui.Style.radiusV
                            color: Qt.tint(control.Maui.Theme.textColor, Qt.rgba(control.Maui.Theme.backgroundColor.r, control.Maui.Theme.backgroundColor.g, control.Maui.Theme.backgroundColor.b, 0.9))
                            border.color: Maui.Theme.backgroundColor
                        }
                        
                        Rectangle
                        {
                            anchors.fill: parent
                            anchors.topMargin: Maui.Style.space.small
                            border.color: Maui.Theme.backgroundColor
                            
                            radius: Maui.Style.radiusV
                            color: Qt.tint(control.Maui.Theme.textColor, Qt.rgba(control.Maui.Theme.backgroundColor.r, control.Maui.Theme.backgroundColor.g, control.Maui.Theme.backgroundColor.b, 0.9))
                            
                            Maui.GridItemTemplate
                            {
                                anchors.fill: parent
                                anchors.margins: Maui.Style.space.tiny
                                iconSizeHint: _info.iconSizeHint
                                
                                iconSource: _info.iconSource
                                imageSource:  _info.imageSource
                            }
                        }
                    }
                }
            }
        }
    ]
    
    page.footer: FB.TagList
    {
        id: tagListComposer
        width: parent.width
        visible: count > 0
        
        onTagRemoved: list.remove(index)
        placeholderText: i18nd("mauikitfilebrowsing", "No tags yet.")
    }
    
    onClosed:
    {
        composerList.urls = []
        tagText.clear()
        _tagsModel.filter = ""
    }
    
    onOpened: tagText.forceActiveFocus()
    
    /**
     * 
     */
    function setTags()
    {
        var tags = []
        
        for(var i = 0; i < tagListComposer.count; i++)
            tags.push(tagListComposer.list.get(i).tag)
            control.tagsReady(tags)
            close()
    }
}
