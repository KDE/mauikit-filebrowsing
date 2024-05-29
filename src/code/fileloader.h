/***
 * Copyright (C) 2018 Camilo Higuita
 * This program comes with ABSOLUTELY NO WARRANTY; for details type `show w'.
 * This is free software, and you are welcome to redistribute it
 * under certain conditions; type `show c' for details.
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
 ***/
#pragma once

#include <QDir>
#include <QObject>
#include <QThread>
#include <QUrl>

#include <MauiKit4/Core/fmh.h>

#include "filebrowsing_export.h"

namespace FMH
{
    /**
     * @brief The FileLoader class asynchronously loads batches of files from a given list of local directories or tags, allowing to filter them by name, mime-types, and much more.
     * 
     * The execution of the file listing will be moved into a different thread, - so to retrieve the information you will depend on the exposed signals and the `informer` callback function to give structure to the data. 
     * @see informer
     * 
     * The following code snippet demonstrates the usage, by listing all the content in the downloads and pictures local directories.
     * 
     * @code
     * #include <QCoreApplication>
     * #include <QDebug>
     * #include <QUrl>
     * #include <QObject>
     * #include <iostream>
     * 
     * #include <MauiKit4/FileBrowsing/fileloader.h>
     * #include <MauiKit4/FileBrowsing/fmstatic.h>
     * 
     * int main(int argc, char *argv[])
     * {
     *    QCoreApplication a(argc, argv);
     * 
     *    FMH::FileLoader loader;
     *    QStringList urls = {FMStatic::DownloadsPath, FMStatic::PicturesPath};
     * 
     *    QObject::connect(&loader, &FMH::FileLoader::itemsReady, [=](FMH::MODEL_LIST items, QList<QUrl> urls) {
     *                         for(const auto &item : items)
     *                             qDebug() << item[FMH::MODEL_KEY::NAME];
     * 
     *                         qDebug() << "items ready for:" << urls << items.length(); });
     * 
     *    QObject::connect(&loader, &FMH::FileLoader::finished, [=](FMH::MODEL_LIST items, QList<QUrl> urls) {
     *          qDebug() << "Finished process" << urls << items.length(); });
     * 
     *    loader.setBatchCount(10);
     *    loader.requestPath(QUrl::fromStringList(urls), true);
     * 
     *    return a.exec();
     * }
     * @endcode
     * 
     */
    class FILEBROWSING_EXPORT FileLoader : public QObject
    {
        Q_OBJECT
        
    public:
        
        /**
         * @brief Creates a new instance, the execution will be moved to a different thread.
         */
        FileLoader(QObject *parent = nullptr);
        ~FileLoader();
        
        /**
         * @brief Set the amount of items to be dispatched via the `itemReady` signal.
         * This allows to dispatch item files which are ready and don't have to wait for the operation to finished completely, in case there are too many files to wait for.
         * @see itemsReady
         * @param count the amount of items
         */
        void setBatchCount(const uint &count);
        
        /**
         * @brief The amount of items which will be dispatched while iterating throughout all the given directories.
         */
        uint batchCount() const;
        
        /**
         * @brief Sends the request to start iterating throughout all the given location URLs, and with the given parameters. 
         * @param urls the list of directories or locations to iterate. This operation only supports local directories and tags.
         * @param recursive Whether the iteration should be done recursively and navigate sub-folder structures
         * @param nameFilters a list of filtering strings, this can be used with regular expressions, for example `*.jpg` to only list files ending with the given suffix. Dy default this is set to an empty array, so nothing will be filtered.
         * @param filters the possible QDir filters. By default this is set to `QDir::Files`.
         * @param limit the limit of files to retrieve. By default this is is set to `99999`.
         */
        void requestPath(const QList<QUrl> &urls, const bool &recursive, const QStringList &nameFilters = {}, const QDir::Filters &filters = QDir::Files, const uint &limit = 99999);
        
        /**
         * @brief A callback function which structures the retrieved file URLs, with the required information. This callback function will receive the file URL, and expects a FMH::MODEL to be formed and returned. By default this informer callback function is set to `FMStatic::getFileInfoModel`, which retrieves basic information about a file.
         * @see FMStatic::getFileInfoModel
         * @param url the file URL retrieved
         * @return the formed data model based on the given file URL
         */
        static std::function<FMH::MODEL(const QUrl &url)> informer;
        
    private Q_SLOTS:
        /**
         * @private
         */
        void getFiles(QList<QUrl> paths, bool recursive, const QStringList &nameFilters, const QDir::Filters &filters, uint limit);
        
    Q_SIGNALS:
        /**
         * @brief Emitted once the operation has completely finished retrieving all the existing files or reached the limit number of requested files.
         * @param items all of the retrieved items. The items data model is created using the `informer` callback function. 
         * @see informer
         * @param urls the list of directories given for which the item were retrieved.
         */
        void finished(FMH::MODEL_LIST items, QList<QUrl> urls);
        
        /**
         * @private
         */
        void start(QList<QUrl> urls, bool recursive, QStringList nameFilters, QDir::Filters filters, uint limit);
        
        /**
         * @brief Emitted when the batch of file items is ready.
         * @see setBatchCount
         * @param items the packaged list of items, formed by the `informer` callback function.
         * @param urls the list of directories given for which the item were retrieved.
         */
        void itemsReady(FMH::MODEL_LIST items, QList<QUrl> urls);
        
        /**
         * @brief Emitted for every single item that becomes available.
         * @param item the packaged item, formed by the `informer` callback function.
         * @param urls the list of directories given for which the item were retrieved.
         */
        void itemReady(FMH::MODEL item, QList<QUrl> urls);
        
    private:
        QThread *m_thread;
        uint m_batchCount = 1500;
    };
}

