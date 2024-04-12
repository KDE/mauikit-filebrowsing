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

import QtQml
import org.mauikit.filebrowsing as FB

/**
 * @inherit QtQml.QtObject
 * @brief A group of properties to tweak the browser in the FileBrowser component.
 * 
 * @warning This control is private and only exposed as part of the FileBrowser implementation. Can not be instantiated.
 */
QtObject
{
  /**
   * @brief A list of string text to filter the current location entries.
   */
  property var filters : []
  
  /**
   * @brief Filter the content by mime type. 
   * By default this is set to `FMList.NONE`, so there is not any type of mimetype filtering
   * @see FMList::FILTER
   */
  property int filterType : FB.FMList.NONE
  
  /**
   * @brief Whether only directories should be listed.
   * By default this is set to `false`
   */
  property bool onlyDirs : false
  
  /**
   * @brief The sorting field.
   * By default this is set to `FMList.LABEL`, which will sort the entries alphabetically by their file names. 
   * @see FMList::SORTBY
   */
  property int sortBy : FB.FMList.LABEL
  
  /**
   * @brief Whether to show the thumbnail previews of images, videos, text files, and other supported types.
   * By default this is set to `true`
   */
  property bool showThumbnails: true
  
  /**
   * @brief Whether to show the hidden file entries.
   * By default this is set to `false`
   */
  property bool showHiddenFiles: false
  
  /**
   * @brief Whether to group the entries by the sort type
   * @see sortBy
   * By default this is set to `false`. Grouping the entries will make thew view go to the list view type.
   */
  property bool group : false
  
  /**
   * @brief The preferred view type, Can be a list or a grid.
   * By default this is set to `FMList.ICON_VIEW`
   * @see FMList::VIEW_TYPE
   */
  property int viewType : 0
  
  /**
   * @brief Whether the folders should be sorted first and then the files.
   * By default this is set to `true`
   */
  property bool foldersFirst: true
  
  /**
   * @brief Whether the search bar should be visible
   * By default this is set to `false`
   */
  property bool searchBarVisible : false    
}

