/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  camilo <milo.h@aol.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <QObject>
#include <QModelIndex>
#include <QHash>
#include <QQmlEngine>

#include "fmstatic.h"

#include <MauiKit4/Core/mauilist.h>

#include "filebrowsing_export.h"

class KFilePlacesModel;

/**
 * @brief The list model of the system locations, such as bookmarks, standard places, networks and devices.
 * 
 * A graphical interface for this controller functionality is available for quick usage as PlacesListBrowser.
 */
class FILEBROWSING_EXPORT PlacesList : public MauiList
{
    Q_OBJECT
    QML_ELEMENT
    Q_DISABLE_COPY(PlacesList)
    
    /**
     * The groups to be listed.
     * The possible list of groups are defined at FMList::PATHTYPE
     * To set it from QML:
     * @code
     * list.groups:  [FMList.BOOKMARKS_PATH, FMList.REMOTE_PATH, FMList.CLOUD_PATH, FMList.DRIVES_PATH]
     * @endcode
     */
    Q_PROPERTY(QVariantList groups READ getGroups WRITE setGroups NOTIFY groupsChanged)

public:
    PlacesList(QObject *parent = nullptr);

    /**
     * @private
     */
    const FMH::MODEL_LIST &items() const override;

    QVariantList getGroups() const;
    void setGroups(const QVariantList &value);

    /**
     * @private
     */
    void componentComplete() override final;

protected:
    void setList();

public Q_SLOTS:
    /**
     * @brief Removes a place from the model and if the data at the given index is a file URL bookmark then it gets removed from the bookmarks.
     * @param index index of the item to be removed in the model
     */
    void removePlace(const int &index);

    /**
     * @brief Checks of a file URL exists in the places model
     * @param path file URL to be checked
     * @return Whether it exists
     */
    bool contains(const QUrl &path);

    /**
     * @brief Check if a entry at the given index is a device
     * @param index index position of the entry in the list
     * @return whether it is a device type
     */
    bool isDevice(const int &index);

    /**
     * @brief Check if a device type entry needs to be setup, as in mounted.
     * @param index the index position of the entry
     * @return whether it needs to be setup
     */
    bool setupNeeded(const int &index);

    /**
     * @brief Request to eject a removable device type at the given index
     * @param index the index position of the entry
     */
    void requestEject (const int &index);

    /**
     * @brief Request to setup or mount the device type entry at the given index
     * @param index index position of the entry
     */
    void requestSetup (const int &index);
    
    /**
     * @brief Add a location to the bookmarks sections
     * @param url The URL path of the location or directory
     */
    static void addBookmark(const QUrl &url);

    /**
     * @brief Given an URL path, if it exists in the places list return its index position
     * @param url The URL path to be checked
     * @return the index position if it exists otherwise `-1`
     */
    int indexOfPath(const QUrl &url) const;
    
    /**
     * @brief Hide/show a section
     * @param The section type to be toggle. The possible values are defined in FMStatic::PATHTYPE_KEY. 
     */
    void toggleSection(const int &section);
    
    /**
     * @brief Whether the current listing contains a group type. The possible values are defined in FMStatic::PATHTYPE_KEY
     * @param group the group type
     * @return whether it is being listed
     */
    bool containsGroup(const int &group);

private:
    FMH::MODEL_LIST list;
    KFilePlacesModel *model;

    QVariantList groups;

    QHash<QString, QModelIndex> m_devices;

    FMH::MODEL_LIST getGroup(const KFilePlacesModel &model, const FMStatic::PATHTYPE_KEY &type);

Q_SIGNALS:
    void groupsChanged();
    
    /**
     * @brief Emitted when a new bookmark entry has been added
     */
    void bookmarksChanged();
};
