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

#pragma once

#include <QObject>

#include <QString>
#include <QStringList>
#include <QVariantMap>
#include <QSqlDatabase>

#include <MauiKit4/Core/fmh.h>

#include "fmstatic.h"

#include "filebrowsing_export.h"

/**
 * @private
 */
namespace TAG
{
    enum class TABLE : uint8_t { APP_TAGS, TAGS, TAGS_URLS, APPS, NONE };
    
    static const QMap<TABLE, QString> TABLEMAP = {{TABLE::TAGS, QStringLiteral("tags")},
    {TABLE::TAGS_URLS, QStringLiteral("tags_urls")},
    {TABLE::APP_TAGS, QStringLiteral("app_tags")},
    {TABLE::APPS, QStringLiteral("apps")}};
    
    static const QString TaggingPath = FMStatic::DataPath + QStringLiteral("/maui/tagging/");
    static const QString DBName = QStringLiteral("tagging-v2.db");
}

/**
 * @brief The TAGDB class exposes methods to add, remove and modify tags in the MauiKit FileBrowsing Tagging system.
 * @warning This class should not be used- use the Tagging object instead, which already wraps this class and exposes most of the functionality needed. In case some functionality is missing, instead of using this class, open a merge request to add the missing functionality to the Tagging class.
 */
class FILEBROWSING_EXPORT TAGDB : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(TAGDB)
    
private:
    QString name;
    QSqlDatabase m_db;    
    
    void openDB(const QString &name);

    void prepareCollectionDB() const;

   const QSqlDatabase& db() const;
   
public:    

   /**
    * @private
    */
    TAGDB();

    /**
     * @private
     */
    ~TAGDB();
    
    //Utils
    /**
     * @brief Check for the existence of an entry
     * @param tableName the name of the table
     * @param searchId the search query
     * @param search the search value
     * @return whether the entry exists
     */
    bool checkExistance(const QString &tableName, const QString &searchId, const QString &search) const;

    /**
     * @brief Check if a entry exists given a query
     * @param queryStr the plain string query
     * @return whether the entry exists
     */
    bool checkExistance(const QString &queryStr) const;

    /**
     * @brief Retrieve the database query object of a performed a query
     * @param queryTxt the query to perform
     * @return the results
     */
    QSqlQuery getQuery(const QString &queryTxt) const;

    /**
     * @brief Return an empty query object to use arbitrary with any query.
     */
    QSqlQuery getQuery() const;

    /**
     * @brief Insert data into the given table
     * @param tableName table name
     * @param insertData the data map to be inserted
     * @return whether the operation was successful
     */
    bool insert(const QString &tableName, const QVariantMap &insertData) const;

    /**
     * @brief Update data in the database
     * @param tableName the table name
     * @param updateData the updated data 
     * @param where the key-value to match in the database
     * @return whether the operation was successful
     */
    bool update(const QString &tableName, const FMH::MODEL &updateData, const QVariantMap &where) const;

    /**
     * @brief Update data in the database
     * @param table table name
     * @param column the column name
     * @param newValue the new value
     * @param op the operation to match
     * @param id the value of the operation `op`
     * @return whether the operation was successful
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

