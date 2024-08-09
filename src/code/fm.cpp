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

#include "fm.h"

#ifdef COMPONENT_SYNCING
#include "syncing.h"
#endif

#include <QDateTime>
#include <QFileInfo>
#include <QLocale>
#include <QRegularExpression>
#include <QUrl>
#include <QDebug>

#ifdef KIO_AVAILABLE
#include <KCoreDirLister>
#include <KFileItem>
#include <KFilePlacesModel>
#include <KIO/CopyJob>
#include <KIO/DeleteJob>
#include <KIO/MkdirJob>
#include <KIO/SimpleJob>
#include <QIcon>
#else

#include "fileloader.h"
#include <QFileSystemWatcher>

QDirLister::QDirLister(QObject *parent)
    : QObject(parent)
    , m_loader(new FMH::FileLoader)
    , m_watcher(new QFileSystemWatcher(this))
{
    m_loader->setBatchCount(20);
    m_loader->informer = &FMStatic::getFileInfoModel;
    connect(m_loader, &FMH::FileLoader::itemsReady, [this](FMH::MODEL_LIST items, QList<QUrl> urls) {
        Q_EMIT this->itemsReady(items, urls.first());
    });

    connect(m_loader, &FMH::FileLoader::itemReady, [this](FMH::MODEL item, QList<QUrl> urls) {
        this->m_list << item;
        this->m_watcher->addPath(QUrl(item[FMH::MODEL_KEY::URL]).toLocalFile());
        Q_EMIT this->itemReady(item, urls.first());
    });

    connect(m_loader, &FMH::FileLoader::finished, [this](FMH::MODEL_LIST, QList<QUrl> urls) {
        Q_EMIT this->completed(urls.first());
    });

    connect(this->m_watcher, &QFileSystemWatcher::directoryChanged, [&](const QString &path) {
        if (path == this->m_url.toLocalFile()) {
            this->reviewChanges();
        }
    });

    connect(this->m_watcher, &QFileSystemWatcher::fileChanged, [&](const QString &path) {
        const auto fileUrl = QUrl::fromLocalFile(path);
        if (this->includes(fileUrl)) {
            if (FMH::fileExists(fileUrl)) {
                Q_EMIT this->refreshItems({{this->m_list.at(this->indexOf(FMH::MODEL_KEY::URL, fileUrl.toString())), FMStatic::getFileInfoModel(fileUrl)}}, this->m_url);
            }
        }
    });
}

void QDirLister::reviewChanges()
{
    if (this->m_checking)
        return;

    this->m_checking = true;
    auto checkLoader = new FMH::FileLoader;
    checkLoader->informer = &FMStatic::getFileInfoModel;

    qDebug() << "Doign the check" << m_checking;

    FMH::MODEL_LIST removedItems;
    const auto mlist = this->m_list; // use a copy to not affect the list indexes on the iterations
    for (const auto &item : std::as_const(mlist)) {
        const auto fileUrl = QUrl(item[FMH::MODEL_KEY::URL]);

        if (!FMH::fileExists(fileUrl)) {
            const auto index = this->indexOf(FMH::MODEL_KEY::URL, fileUrl.toString());

            if (index < this->m_list.count() && index >= 0) {
                removedItems << item;
                this->m_list.remove(index);
                this->m_watcher->removePath(fileUrl.toLocalFile());
            }
        }
    }

    if (!removedItems.isEmpty())
        Q_EMIT this->itemsDeleted(removedItems, this->m_url);

    connect(checkLoader, &FMH::FileLoader::itemsReady, [=](FMH::MODEL_LIST items, QList<QUrl> urls) {
        if (urls.first() == this->m_url) {
            FMH::MODEL_LIST newItems;
            for (const auto &item : std::as_const(items)) {
                const auto fileUrl = QUrl(item[FMH::MODEL_KEY::URL]);
                if (!this->includes(fileUrl)) {
                    newItems << item;

                    this->m_list << item;
                    this->m_watcher->addPath(fileUrl.toLocalFile());
                }
            }

            if (!newItems.isEmpty())
                Q_EMIT this->itemsAdded(newItems, this->m_url);
        }

        checkLoader->deleteLater();
        this->m_checking = false;
    });

    connect(checkLoader, &FMH::FileLoader::finished, [=](FMH::MODEL_LIST, QList<QUrl>) {
        checkLoader->deleteLater();
        this->m_checking = false;
    });

    QDir::Filters dirFilter = (m_dirOnly ? QDir::AllDirs | QDir::NoDotDot | QDir::NoDot : QDir::Files | QDir::AllDirs | QDir::NoDotDot | QDir::NoDot);

    if (m_showDotFiles)
        dirFilter = dirFilter | QDir::Hidden | QDir::System;

    checkLoader->requestPath({this->m_url}, false, m_nameFilters.isEmpty() ? QStringList() : m_nameFilters.split(QStringLiteral(" ")), dirFilter);
}

bool QDirLister::includes(const QUrl &url)
{
    return this->indexOf(FMH::MODEL_KEY::URL, url.toString()) >= 0;
}

int QDirLister::indexOf(const FMH::MODEL_KEY &key, const QString &value) const
{
    const auto items = this->m_list;
    const auto it = std::find_if(items.constBegin(), items.constEnd(), [&](const FMH::MODEL &item) -> bool {
        return item[key] == value;
    });

    if (it != items.constEnd())
        return std::distance(items.constBegin(), it);
    else
        return -1;
}

bool QDirLister::openUrl(const QUrl &url)
{
    //    if(this->m_url == url)
    //        return false;

    qDebug() << "open URL" << url;
    this->m_url = url;
    this->m_list.clear();
    this->m_watcher->removePaths(QStringList() << this->m_watcher->directories() << this->m_watcher->files());

    if (FMStatic::isDir(this->m_url)) {
        this->m_watcher->addPath(this->m_url.toLocalFile());

        QDir::Filters dirFilter = QDir::AllDirs | QDir::NoDotDot | QDir::NoDot;

        dirFilter.setFlag(QDir::Hidden, m_showDotFiles);
        dirFilter.setFlag(QDir::Files, !m_dirOnly);

        m_loader->requestPath({this->m_url}, false, m_nameFilters.isEmpty() ? QStringList() : m_nameFilters.split(QStringLiteral(" ")), dirFilter);

    } else
        return false;

    return true;
}

void QDirLister::setDirOnlyMode(bool value)
{
    m_dirOnly = value;
}

void QDirLister::setShowingDotFiles(bool value)
{
    m_showDotFiles = value;
}

void QDirLister::setShowHiddenFiles(bool value)
{
    setShowingDotFiles(value);
}

void QDirLister::setNameFilter(QString filters)
{
    m_nameFilters = filters;
}
#endif

FM::FM(QObject *parent)
    : QObject(parent)
#ifdef COMPONENT_SYNCING
    , sync(new Syncing(this))
#endif
#ifdef KIO_AVAILABLE
    , dirLister(new KCoreDirLister(this))
#else
    , dirLister(new QDirLister(this))
#endif
{

#ifdef KIO_AVAILABLE
    this->dirLister->setDelayedMimeTypes(true);
    this->dirLister->setAutoUpdate(true);

    const static auto packItems = [](const KFileItemList &items) -> FMH::MODEL_LIST {
        return std::accumulate(items.constBegin(), items.constEnd(), FMH::MODEL_LIST(), [](FMH::MODEL_LIST &res, const KFileItem &item) -> FMH::MODEL_LIST {
            res << FMStatic::getFileInfo(item);
            return res;
        });
    };

    connect(dirLister, static_cast<void (KCoreDirLister::*)(const QUrl &)>(&KCoreDirLister::listingDirCompleted), this, [&](QUrl url) {
        qDebug() << "PATH CONTENT READY" << url;
        Q_EMIT this->pathContentReady(url);
    });
    
    connect(dirLister, static_cast<void (KCoreDirLister::*)(const QUrl &)>(&KCoreDirLister::listingDirCanceled), this, [&](QUrl url) {
        qDebug() << "PATH CONTENT READY" << url;
        Q_EMIT this->pathContentReady(url);
    });

    connect(dirLister, static_cast<void (KCoreDirLister::*)(const QUrl &, const KFileItemList &items)>(&KCoreDirLister::itemsAdded), this, [&](QUrl dirUrl, KFileItemList items) {
        qDebug() << "MORE ITEMS WERE ADDED";
        Q_EMIT this->pathContentItemsReady({dirUrl, packItems(items)});
    });

//        connect(dirLister, static_cast<void (KCoreDirLister::*)(const KFileItemList &items)>(&KCoreDirLister::newItems), [&](KFileItemList items)
//        {
//            qDebug()<< "MORE NEW ITEMS WERE ADDED";
//            for(const auto &item : items)
//                qDebug()<< "MORE <<" << item.url();
//     
//            Q_EMIT this->pathContentChanged(dirLister->url());
//        });

    connect(dirLister, static_cast<void (KCoreDirLister::*)(const KFileItemList &items)>(&KCoreDirLister::itemsDeleted), this, [&](KFileItemList items) {
        qDebug() << "ITEMS WERE DELETED";
        Q_EMIT this->pathContentItemsRemoved({dirLister->url(), packItems(items)});
    });

    connect(dirLister, static_cast<void (KCoreDirLister::*)(const QList<QPair<KFileItem, KFileItem>> &items)>(&KCoreDirLister::refreshItems), this, [&](QList<QPair<KFileItem, KFileItem>> items) {
        qDebug() << "ITEMS WERE REFRESHED";

        const auto res = std::accumulate(
            items.constBegin(), items.constEnd(), QVector<QPair<FMH::MODEL, FMH::MODEL>>(), [](QVector<QPair<FMH::MODEL, FMH::MODEL>> &list, const QPair<KFileItem, KFileItem> &pair) -> QVector<QPair<FMH::MODEL, FMH::MODEL>> {
                list << QPair<FMH::MODEL, FMH::MODEL> {FMStatic::getFileInfo(pair.first), FMStatic::getFileInfo(pair.second)};
                return list;
            });

        Q_EMIT this->pathContentItemsChanged(res);
    });
#else
    connect(dirLister, &QDirLister::itemsReady, this, [&](FMH::MODEL_LIST items, QUrl url) {
        Q_EMIT this->pathContentItemsReady({url, items});
    });

    connect(dirLister, &QDirLister::completed, this, [&](QUrl url) {
        Q_EMIT this->pathContentReady(url);
    });

    connect(dirLister, &QDirLister::refreshItems, this, [&](QVector<QPair<FMH::MODEL, FMH::MODEL>> items, QUrl) {
        qDebug() << "ITEMS WERE REFRESHED";
        Q_EMIT this->pathContentItemsChanged(items);
    }); 

    connect(dirLister, &QDirLister::itemsAdded, this, [&](FMH::MODEL_LIST items, QUrl url) {
        qDebug() << "MORE ITEMS WERE ADDED";
        Q_EMIT this->pathContentItemsReady({url, items});
        Q_EMIT this->pathContentReady(url); //Q_EMIT this to force to sort on fmlist
    });

    connect(dirLister, &QDirLister::itemsDeleted, this, [&](FMH::MODEL_LIST items, QUrl url) {
        qDebug() << "ITEMS WERE DELETED";
        Q_EMIT this->pathContentItemsRemoved({url, items});
    });

#endif

#ifdef COMPONENT_SYNCING
    connect(this->sync, &Syncing::listReady, [this](const FMH::MODEL_LIST &list, const QUrl &url) {
        Q_EMIT this->cloudServerContentReady({url, list});
    });

    connect(this->sync, &Syncing::itemReady, [this](const FMH::MODEL &item, const QUrl &url, const Syncing::SIGNAL_TYPE &signalType) {
        switch (signalType) {
        case Syncing::SIGNAL_TYPE::OPEN:
            FMStatic::openUrl(item[FMH::MODEL_KEY::PATH]);
            break;

        case Syncing::SIGNAL_TYPE::DOWNLOAD:
            Q_EMIT this->cloudItemReady(item, url);
            break;

        case Syncing::SIGNAL_TYPE::COPY: {
            QVariantMap data;
            const auto keys = item.keys();
            for (auto key : keys)
                data.insert(FMH::MODEL_NAME[key], item[key]);

            //                         this->copy(QVariantList {data}, this->sync->getCopyTo());
            break;
        }
        default:
            return;
        }
    });

    connect(this->sync, &Syncing::error, [this](const QString &message) {
        Q_EMIT this->warningMessage(message);
    });

    connect(this->sync, &Syncing::progress, [this](const int &percent) {
        Q_EMIT this->loadProgress(percent);
    });

    connect(this->sync, &Syncing::dirCreated, [this](const FMH::MODEL &dir, const QUrl &url) {
        Q_EMIT this->newItem(dir, url);
    });

    connect(this->sync, &Syncing::uploadReady, [this](const FMH::MODEL &item, const QUrl &url) {
        Q_EMIT this->newItem(item, url);
    });
#endif
}

void FM::getPathContent(const QUrl &path, const bool &hidden, const bool &onlyDirs, const QStringList &filters, const QDirIterator::IteratorFlags &iteratorFlags)
{
    qDebug() << "Getting async path contents";
    Q_UNUSED(iteratorFlags)

    this->dirLister->setShowHiddenFiles(hidden);
    this->dirLister->setDirOnlyMode(onlyDirs);
    this->dirLister->setNameFilter(filters.join(QStringLiteral(" ")));

    if (this->dirLister->openUrl(path))
        qDebug() << "GETTING PATH CONTENT" << path;
}

bool FM::getCloudServerContent(const QUrl &path, const QStringList &filters, const int &depth)
{
    Q_UNUSED(path)
    Q_UNUSED(filters)    
    Q_UNUSED(depth)    
    
#ifdef COMPONENT_SYNCING
    const auto __list = path.toString().replace("cloud:///", "/").split("/");

    if (__list.isEmpty() || __list.size() < 2) {
        qWarning() << "Could not parse username to get cloud server content";
        return false;
    }

    auto user = __list[1];
    //        auto data = this->get(QString("select * from clouds where user = '%1'").arg(user));
    QVariantList data;
    if (data.isEmpty())
        return false;

    auto map = data.first().toMap();

    user = map[FMH::MODEL_NAME[FMH::MODEL_KEY::USER]].toString();
    auto server = map[FMH::MODEL_NAME[FMH::MODEL_KEY::SERVER]].toString();
    auto password = map[FMH::MODEL_NAME[FMH::MODEL_KEY::PASSWORD]].toString();
    this->sync->setCredentials(server, user, password);

    this->sync->listContent(path, filters, depth);
    return true;
#else
    return false;
#endif
}

void FM::createCloudDir(const QString &path, const QString &name)
{
    Q_UNUSED(path)
    Q_UNUSED(name)
    
#ifdef COMPONENT_SYNCING
    this->sync->createDir(path, name);
#endif
}

void FM::openCloudItem(const QVariantMap &item)
{
    Q_UNUSED(item)
    
#ifdef COMPONENT_SYNCING
    this->sync->resolveFile(FMH::toModel(item), Syncing::SIGNAL_TYPE::OPEN);
#endif
}

void FM::getCloudItem(const QVariantMap &item)
{
    Q_UNUSED(item)    
#ifdef COMPONENT_SYNCING
    this->sync->resolveFile(FMH::toModel(item), Syncing::SIGNAL_TYPE::DOWNLOAD);
#endif
}

QString FM::resolveUserCloudCachePath(const QString &server, const QString &user)
{
    Q_UNUSED(server)
    return FMStatic::CloudCachePath + QStringLiteral("opendesktop/") + user;
}

QString FM::resolveLocalCloudPath(const QString &path)
{
    Q_UNUSED(path)
#ifdef COMPONENT_SYNCING
    return QString(path).replace(FMStatic::PATHTYPE_URI[FMStatic::PATHTYPE_KEY::CLOUD_PATH] + this->sync->getUser(), "");
#else
    return QString();
#endif
}

bool FM::cut(const QList<QUrl> &urls, const QUrl &where)
{
    return FMStatic::cut(urls, where);
}

bool FM::copy(const QList<QUrl> &urls, const QUrl &where)
{
    // 	QStringList cloudPaths;

    return FMStatic::copy(urls, where);

#ifdef COMPONENT_SYNCING
    // 	if(!cloudPaths.isEmpty())
    // 	{
    // 		qDebug()<<"UPLOAD QUEUE" << cloudPaths;
    // 		const auto firstPath = cloudPaths.takeLast();
    // 		this->sync->setUploadQueue(cloudPaths);
    //
    // 		if(where.toString().split("/").last().contains("."))
    // 		{
    // 			QStringList whereList = where.toString().split("/");
    // 			whereList.removeLast();
    // 			auto whereDir = whereList.join("/");
    // 			qDebug()<< "Trying ot copy to cloud" << where << whereDir;
    //
    // 			this->sync->upload(this->resolveLocalCloudPath(whereDir), firstPath);
    // 		} else
    // 			this->sync->upload(this->resolveLocalCloudPath(where.toString()), firstPath);
    // 	}
#endif
}
