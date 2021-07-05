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

#ifndef TAGDB_H
#define TAGDB_H


#include <QObject>
#include <QSqlDatabase>

#include <QString>
#include <QStringList>
#include <QVariantMap>

#include <MauiKit/Core/fmh.h>

#include "fmstatic.h"

#include "filebrowsing_export.h"

namespace TAG
{
    enum class TABLE : uint8_t { APP_TAGS, TAGS, TAGS_URLS, APPS, NONE };
    
    static const QMap<TABLE, QString> TABLEMAP = {{TABLE::TAGS, "tags"},
    {TABLE::TAGS_URLS, "tags_urls"},
    {TABLE::APP_TAGS, "app_tags"},
    {TABLE::APPS, "apps"}};
    
    static const QString TaggingPath = FMStatic::DataPath + "/maui/tagging/";
    static const QString DBName = "tagging-v2.db";
}


/**
 * @brief The TAGDB class
 */
class FILEBROWSING_EXPORT TAGDB : public QObject
{
    Q_OBJECT
private:
    QString name;
    QSqlDatabase m_db;

public:
    /* utils*/
    /**
     * @brief checkExistance
     * @param tableName
     * @param searchId
     * @param search
     * @return
     */
    bool checkExistance(const QString &tableName, const QString &searchId, const QString &search) const;

    /**
     * @brief checkExistance
     * @param queryStr
     * @return
     */
    bool checkExistance(const QString &queryStr) const;


    TAGDB();
    ~TAGDB();

    /**
     * @brief getQuery
     * @param queryTxt
     * @return
     */
    QSqlQuery getQuery(const QString &queryTxt) const;

    /**
     * @brief openDB
     * @param name
     */
    void openDB(const QString &name);

    /**
     * @brief prepareCollectionDB
     */
    void prepareCollectionDB() const;

    /**
     * @brief insert
     * @param tableName
     * @param insertData
     * @return
     */
    bool insert(const QString &tableName, const QVariantMap &insertData) const;

    /**
     * @brief update
     * @param tableName
     * @param updateData
     * @param where
     * @return
     */
    bool update(const QString &tableName, const FMH::MODEL &updateData, const QVariantMap &where) const;

    /**
     * @brief update
     * @param table
     * @param column
     * @param newValue
     * @param op
     * @param id
     * @return
     */
    bool update(const QString &table, const QString &column, const QVariant &newValue, const QVariant &op, const QString &id) const;

    /**
     * @brief remove
     * @param tableName
     * @param removeData
     * @return
     */
    bool remove(const QString &tableName, const FMH::MODEL &removeData) const;
};

#endif // DB_H
