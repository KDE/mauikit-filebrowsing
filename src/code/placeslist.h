/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  camilo <email>
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

#include "fmstatic.h"

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <MauiKit3/Core/mauilist.h>
#else
#include <MauiKit4/Core/mauilist.h>
#endif

class KFilePlacesModel;

class PlacesList : public MauiList
{
    Q_OBJECT
    Q_DISABLE_COPY(PlacesList)
    
    Q_PROPERTY(QVariantList groups READ getGroups WRITE setGroups NOTIFY groupsChanged)

public:
    PlacesList(QObject *parent = nullptr);

    const FMH::MODEL_LIST &items() const override;

    QVariantList getGroups() const;
    void setGroups(const QVariantList &value);

    void componentComplete() override final;

protected:
    void setList();

public Q_SLOTS:
    /**
     * @brief removePlace
     * Removes a place from the model and if the data at the given index is a file URL bookmark then it gets removed from the bookmarks.
     * @param index
     * Index of the item to be removed in the model
     */
    void removePlace(const int &index);

    /**
     * @brief contains
     * Checks of a file URL exists in the places model
     * @param path
     * File URL to be checked
     * @return
     * True if it exists otherwise false
     */
    bool contains(const QUrl &path);

    bool isDevice(const int &index);

    bool setupNeeded(const int &index);

    void requestEject (const int &index);

    void requestSetup (const int &index);
    
    static void addBookmark(const QUrl &url);

    int indexOfPath(const QUrl &url) const;
    
    void toggleSection(const int &section);
    
    bool containsGroup(const int &group);

private:
    FMH::MODEL_LIST list;
    KFilePlacesModel *model;

    QVariantList groups;

    QHash<QString, QModelIndex> m_devices;

    FMH::MODEL_LIST getGroup(const KFilePlacesModel &model, const FMStatic::PATHTYPE_KEY &type);

Q_SIGNALS:
    void groupsChanged();
    void bookmarksChanged();
};
