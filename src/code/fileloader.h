/***
Pix  Copyright (C) 2018  Camilo Higuita
This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
This is free software, and you are welcome to redistribute it
under certain conditions; type `show c' for details.

 This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
***/
#pragma once

#include <QDir>
#include <QObject>
#include <QThread>
#include <QUrl>

#include <MauiKit/Core/fmh.h>
#include "filebrowsing_export.h"

namespace FMH
{
/**
 * @brief The FileLoader class
 * Allows to asyncronously load files from a given list of directories, allowing to filter by mimetype
 */
class FILEBROWSING_EXPORT FileLoader : public QObject
{
    Q_OBJECT
public:
    FileLoader(QObject *parent = nullptr);
    ~FileLoader();

    void setBatchCount(const uint &count);
    uint batchCount() const;

    /**
     * @brief requestPath
     * @param urls
     * @param recursive
     * @param nameFilters
     * @param filters
     * @param limit
     */
    void requestPath(const QList<QUrl> &urls, const bool &recursive, const QStringList &nameFilters = {}, const QDir::Filters &filters = QDir::Files, const uint &limit = 99999);

    static std::function<FMH::MODEL(const QUrl &url)> informer;

private Q_SLOTS:
    /**
     * @brief getFiles
     * @param paths
     * @param recursive
     * @param nameFilters
     * @param filters
     * @param limit
     */
    void getFiles(QList<QUrl> paths, bool recursive, const QStringList &nameFilters, const QDir::Filters &filters, uint limit);
   
Q_SIGNALS:
    /**
     * @brief finished
     * @param items
     */
    void finished(FMH::MODEL_LIST items, QList<QUrl> urls);

    /**
     * @brief start
     * @param urls
     * @param recursive
     * @param nameFilters
     * @param filters
     * @param limit
     */
    void start(QList<QUrl> urls, bool recursive, QStringList nameFilters, QDir::Filters filters, uint limit);

    /**
     * @brief itemsReady
     * @param items
     */
    void itemsReady(FMH::MODEL_LIST items, QList<QUrl> urls);

    /**
     * @brief itemReady
     * @param item
     */
    void itemReady(FMH::MODEL item, QList<QUrl> urls);

private:
    QThread *m_thread;
    uint m_batchCount = 1500;
};
}

