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
import QtQuick.Controls

import org.mauikit.controls as Maui

import "private" as Private

/**
 * @inherit QtQuick.Item
 * @brief A bar to list, add or remove tags for a given set of files.
 * @see TagsList::urls
 * @see list
 *
 * The retrieved file tags can be restricted to only tags created/associated by the app itself or by any app, this can be tweaked via the `list.strict` property.
 * @see list
 *
 * @image html tagsbar.png "Example using TagsBar and FileBrowser controls"
 *
 * @code
 * Maui.Page
 * {
 *    id: _tagPreviewPage
 *    Maui.Controls.showCSD: true
 *    anchors.fill: parent
 *
 *    property var fileItem //A map object representing some properties from a file, such as its name, icon, url, etc.
 *
 *    Maui.Holder
 *    {
 *        anchors.fill: parent
 *        emojiSize: 100
 *        imageSource: _tagPreviewPage.fileItem.thumbnail
 *        title: _tagPreviewPage.fileItem.name
 *    }
 *
 *    footer: FB.TagsBar
 *    {
 *        list.strict: false
 *        list.urls: [_tagPreviewPage.fileItem.url]
 *        width: parent.width
 *
 *        onTagsEdited: (tags) => list.updateToUrls(tags)
 *    }
 * }
 * }
 * @endcode
 *
 * <a href="https://invent.kde.org/maui/mauikit-filebrowser/examples/TagsBar.qml">You can find a more complete example at this link.</a>
 */
Item
{
    id: control
    
    focus: true
    implicitHeight: Maui.Style.toolBarHeight + Maui.Style.space.tiny
    
    /**
     * @brief An alias to the flickable element listing the tag buttons.
     * It is exposed to fine tune more of this control properties.
     * @property Taglist TagsBar::listView
     */
    readonly property alias listView : tagsList
    
    /**
     * @brief The total amount of tag elements
     * @property int TagsBar::count
     */
    readonly property alias count : tagsList.count
    
    /**
     * @brief Whether the bar is in edit mode or not. This can be also triggered by the user using the attached action buttons in the right side of the bar.
     * In edit mode the bar exposes a text field, where all the tag elements are plain text divided by comma. The text can be edited to remove tags or add more.
     * @see allowEditMode
     * By default this is set to `false`.
     */
    readonly property bool editMode : _stackView.depth === 2
    
    /**
     * @brief Whether the bar exposes to the user the action buttons that allow to go into edit mode, or to remove the tag elements manually.
     * By default this is set to `false`
     */
    property bool allowEditMode : false
    
    /**
     * @see TagList::list
     * @brief To associate a one or a group of file URLs, use `list.urls`, or to disable the strict mode use `list.strict: false`, etc. Read more about the available properties in the TagsListModel documentation page.
     * @property TagsListModel TagsBar::list
     */
    readonly property alias list : tagsList.list
    
    /**
     * Emitted when the close/dismiss button of a tag element has been clicked.
     * @param index the index position of the tag element
     *
     * @note To retrieve information of the tag given the index position, use the alias property function`list.get(index)`
     */
    signal tagRemovedClicked(int index)
    
    /**
     * Emitted when a tag element has been clicked.
     * @param tag the name of the tag element
     */
    signal tagClicked(string tag)
    
    /**
     * @brief Emitted when the tags have been manually edited by the user via the text field input.
     * @param tags the list of tags entered in the text field.
     */
    signal tagsEdited(var tags)

    onAllowEditModeChanged:
    {
        if(!control.allowEditMode)
            control.closeEditMode()
    }

    Connections
    {
        target: tagsList.list
        onUrlsChanged: closeEditMode()
    }
    
    StackView
    {
        id: _stackView
        anchors.fill: parent
        
        initialItem: Private.TagList
        {
            id: tagsList
            
            showPlaceHolder: allowEditMode
            showDeleteIcon: allowEditMode
            
            onTagRemoved: (index) => tagRemovedClicked(index)
            
            onTagClicked: (index) => control.tagClicked(tagsList.list.get(index).tag)
            
            onAreaClicked:
            {
                if(allowEditMode)
                {
                    goEditMode()
                }
            }

            Item
            {
                enabled: visible
                visible: Maui.Handy.isMobile
                anchors.fill: parent
                TapHandler
                {
                    onTapped: goEditMode()
                }
            }
        }
        
        Component
        {
            id: _editComponent
            Maui.TextField
            {
                id: editTagsEntry
                
                focus: true
                
                activeFocusOnPress : true
                
                text: list.tags.join(",")
                
                StackView.onActivated:
                {
                    editTagsEntry.forceActiveFocus()
                }
                
                onActiveFocusChanged:
                {
                    if(!editTagsEntry.activeFocus)
                    {
                        closeEditMode()
                    }
                }
                
                onAccepted:
                {
                    control.tagsEdited(getTags())
                    control.closeEditMode()
                }
                
                actions: Action
                {
                    icon.name: "checkbox"
                    onTriggered: editTagsEntry.accepted()
                }
                
                background: Rectangle
                {
                    color: "transparent"
                }
                
                function getTags()
                {
                    if(!editTagsEntry.text.length > 0)
                    {
                        return
                    }
                    
                    var tags = []
                    if(editTagsEntry.text.trim().length > 0)
                    {
                        var list = editTagsEntry.text.split(",")
                        
                        if(list.length > 0)
                        {
                            for(var i in list)
                            {
                                tags.push(list[i].trim())
                                
                            }
                        }
                    }
                    
                    return tags
                }
            }
        }
    }
    
    /**
     * @brief Force the bar to go into editing mode.
     */
    function goEditMode()
    {
        control.forceActiveFocus()
        if(control.allowEditMode)
            _stackView.push(_editComponent)
    }
    
    /**
     * @brief Force to exit the editing mode.
     */
    function closeEditMode()
    {
        if(_stackView.depth === 2)
            _stackView.pop()
    }
}
