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
import QtQuick.Controls 2.14

import org.mauikit.controls 1.2 as Maui

import "private"

/**
 * TagsBar
 * A global sidebar for the application window that can be collapsed.
 *
 *
 *
 *
 *
 *
 */
Item
{
    id: control
    focus: true
    implicitHeight: Maui.Style.toolBarHeight + Maui.Style.space.tiny
    
    /**
     * listView : TagList
     */
    property alias listView : tagsList
    
    /**
     * count : int
     */
    property alias count : tagsList.count
    
    /**
     * editMode : bool
     */
    property bool editMode : false
    
    /**
     * allowEditMode : bool
     */
    property bool allowEditMode : false
    
    /**
     * list : TagsList
     */
    property alias list : tagsList.list
    
    /**
     * addClicked :
     */
    signal addClicked()
    
    /**
     * tagRemovedClicked :
     */
    signal tagRemovedClicked(int index)
    
    /**
     * tagClicked :
     */
    signal tagClicked(string tag)
    
    /**
     * tagsEdited :
     */
    signal tagsEdited(var tags)
    
    Loader
    {
        anchors.fill: parent
        active: control.editMode
        visible: active
        asynchronous: true
        
        sourceComponent: Maui.TextField
        {
            id: editTagsEntry
            focus: true
            
            activeFocusOnPress : true
            
            text: list.tags.join(",")
            
            Component.onCompleted:
            {
                editTagsEntry.forceActiveFocus()            
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
    
    TagList
    {
        id: tagsList
        anchors.fill: parent
        visible: !control.editMode
        showPlaceHolder: allowEditMode
        showDeleteIcon: allowEditMode
        onTagRemoved: tagRemovedClicked(index)
        onTagClicked: control.tagClicked(tagsList.list.get(index).tag)
        
        onAreaClicked:
        {
            if(allowEditMode)
            {
                goEditMode()
            }
        }
    }
    
    /**
     * 
     */
    function goEditMode()
    {
        control.forceActiveFocus()
        control.editMode = true
    }
    
    function closeEditMode()
    {
        control.editMode = false
    }    
}
