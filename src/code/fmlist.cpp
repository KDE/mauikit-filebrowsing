/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  camilo higuita <milo.h@aol.com>
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

#include "fmlist.h"
#include "fm.h"
#include "tagging.h"

#ifdef COMPONENT_SYNCING
#include "syncing.h"
#endif

#include <QFuture>
#include <QThread>
#include <QtConcurrent/QtConcurrentRun>
#include <QtConcurrent>

#include <QClipboard>
#include <QGuiApplication>

#include <KLocalizedString>

FMList::FMList(QObject *parent)
    : MauiList(parent)
    , fm(new FM(this))
{
    qRegisterMetaType<FMList *>("const FMList*"); // this is needed for QML to know of FMList in the search method
    connect(this->fm, &FM::cloudServerContentReady, [this](FMStatic::PATH_CONTENT res) {
        if (this->path == res.path) {
            this->assignList(res.content);
        }
    });

    connect(this->fm, &FM::pathContentReady, [this](QUrl) {
        Q_EMIT this->preListChanged();
        this->sortList();
        this->setStatus({PathStatus::STATUS_CODE::READY, this->list.isEmpty() ? i18n("Nothing here!") : QStringLiteral(""), this->list.isEmpty() ? i18n("This place seems to be empty") : QStringLiteral(""), this->list.isEmpty() ? QStringLiteral("folder-add") : QStringLiteral(""), this->list.isEmpty(), true});
        Q_EMIT this->postListChanged();
        Q_EMIT this->countChanged();
    });

    connect(this->fm, &FM::pathContentItemsChanged, [this](QVector<QPair<FMH::MODEL, FMH::MODEL>> res) {
        for (const auto &item : std::as_const(res)) {
            const auto index = this->indexOf(FMH::MODEL_KEY::PATH, item.first[FMH::MODEL_KEY::PATH]);

            if (index >= this->list.size() || index < 0)
                return;

            this->list[index] = item.second;
            Q_EMIT this->updateModel(index, FMH::modelRoles(item.second));
        }
    });

    connect(this->fm, &FM::pathContentItemsReady, [this](FMStatic::PATH_CONTENT res) {
        if (res.path != this->path)
            return;

        this->appendToList(res.content);
    });

    connect(this->fm, &FM::pathContentItemsRemoved, [this](FMStatic::PATH_CONTENT res) {
        if (res.path != this->path)
            return;

        if (!FMH::fileExists(res.path)) {
            this->setStatus({PathStatus::STATUS_CODE::ERROR, i18n("Error"), i18n("This URL cannot be listed"), QStringLiteral("documentinfo"), true, false});
            return;
        }

        for (const auto &item : std::as_const(res.content)) {
            const auto index = this->indexOf(FMH::MODEL_KEY::PATH, item[FMH::MODEL_KEY::PATH]);
            qDebug() << "SUPOSSED TO REMOVED THIS FORM THE LIST" << index << this->list.count() << item[FMH::MODEL_KEY::PATH];

            this->remove(index);
        }

        this->setStatus({PathStatus::STATUS_CODE::READY, this->list.isEmpty() ? i18n("Nothing here!") : QStringLiteral(""), this->list.isEmpty() ? i18n("This place seems to be empty") : QStringLiteral(""), this->list.isEmpty() ? QStringLiteral("folder-add") : QStringLiteral(""), this->list.isEmpty(), true});
    });

    connect(this->fm, &FM::warningMessage, [this](const QString &message) {
        Q_EMIT this->warning(message);
    });

    connect(this->fm, &FM::loadProgress, [this](const int &percent) {
        Q_EMIT this->progress(percent);
    });

    connect(this->fm, &FM::pathContentChanged, [this](const QUrl &path) {
        qDebug() << "FOLDER PATH CHANGED" << path;
        if (path != this->path)
            return;
        this->sortList();
    });

    connect(this->fm, &FM::newItem, [this](const FMH::MODEL &item, const QUrl &url) {
        if (this->path == url) {
            Q_EMIT this->preItemAppended();
            this->list << item;
            Q_EMIT this->postItemAppended();
            Q_EMIT this->countChanged();
        }
    });
    
    connect(Tagging::getInstance(), &Tagging::urlTagged, [this](QString, QString tag)
    {
        if(this->getPathType() == FMList::PATHTYPE::TAGS_PATH)
        {
            const auto url = this->path.toString();
            if(url.endsWith(tag))
            {
                this->refresh();
            }
        }
    });
    
    connect(Tagging::getInstance(), &Tagging::tagged, [this](QVariantMap)
    {
        if(this->pathType == PATHTYPE::TAGS_PATH)
        {
            this->refresh();
        }
    });
    
    connect(Tagging::getInstance(), &Tagging::tagRemoved, [this](QString)
    {
        if(this->pathType == PATHTYPE::TAGS_PATH)
        {
            this->refresh();
        }
    });
}

void FMList::assignList(const FMH::MODEL_LIST &list)
{
    Q_EMIT this->preListChanged();
    this->list = list;
    this->sortList();
    this->setStatus({PathStatus::STATUS_CODE::READY, this->list.isEmpty() ? i18n("Nothing here!") : QStringLiteral(""), this->list.isEmpty() ? i18n("This place seems to be empty") : QStringLiteral(""), this->list.isEmpty() ? QStringLiteral("folder-add") : QStringLiteral(""), this->list.isEmpty(), true});
    Q_EMIT this->postListChanged();
    Q_EMIT this->countChanged();
}

void FMList::appendToList(const FMH::MODEL_LIST &list)
{
    Q_EMIT this->preItemsAppended(list.size());
    this->list << list;
    Q_EMIT this->postItemAppended();
    Q_EMIT this->countChanged();
}

void FMList::clear()
{
    Q_EMIT this->preListChanged();
    this->list.clear();
    Q_EMIT this->postListChanged();
    Q_EMIT this->countChanged();
}

FMH::MODEL_LIST FMList::getTagContent(const QString &tag, const QStringList &filters)
{   
    if (tag.isEmpty()) {
        return Tagging::getInstance()->getTags();
    } else {
        FMH::MODEL_LIST content;
        const auto urls = Tagging::getInstance()->getTagUrls(tag, filters, false);
        for (const auto &url : urls) {
            content << FMStatic::getFileInfoModel(url);
        }
        
        return content;
    }
}

void FMList::setList()
{
    qDebug() << "PATHTYPE FOR URL" << pathType << this->path.toString() << this->filters << this;
    
    if(this->path.isEmpty() || !m_autoLoad)
    {
        return;
    }
    
    this->clear();

    switch (this->pathType)
    {
    case FMList::PATHTYPE::TAGS_PATH:
        this->assignList(getTagContent(this->path.fileName(), QStringList() << this->filters << FMStatic::FILTER_LIST[static_cast<FMStatic::FILTER_TYPE>(this->filterType)]));
        break; // SYNC

    case FMList::PATHTYPE::CLOUD_PATH:
        this->fm->getCloudServerContent(this->path, this->filters, this->cloudDepth);
        break; // ASYNC

    default: {
        const bool exists = this->path.isLocalFile() ? FMH::fileExists(this->path) : true;
        if (!exists)
            this->setStatus({PathStatus::STATUS_CODE::ERROR, i18n("Error"), i18n("This URL cannot be listed"), QStringLiteral("documentinfo"), this->list.isEmpty(), exists});
        else {
            this->fm->getPathContent(this->path, this->hidden, this->onlyDirs, QStringList() << this->filters << FMStatic::FILTER_LIST[static_cast<FMStatic::FILTER_TYPE>(this->filterType)]);
        }
        break; // ASYNC
    }
    }
}

void FMList::reset()
{
    this->setList();
}

const FMH::MODEL_LIST &FMList::items() const
{
    return this->list;
}

FMList::SORTBY FMList::getSortBy() const
{
    return this->sort;
}

void FMList::setSortBy(const FMList::SORTBY &key)
{
    if (this->sort == key)
        return;

    this->sort = key;
    Q_EMIT this->sortByChanged();
}

void FMList::sortList()
{
    const FMH::MODEL_KEY key = static_cast<FMH::MODEL_KEY>(this->sort);
    auto it = this->list.begin();
    
    const auto sortFunc = [key](const FMH::MODEL &e1, const FMH::MODEL &e2) -> bool
    {
        switch (key) {
        case FMH::MODEL_KEY::SIZE: {
            if (e1[key].toDouble() > e2[key].toDouble())
                return true;
            break;
        }
            
        case FMH::MODEL_KEY::ADDDATE:
        case FMH::MODEL_KEY::MODIFIED:
        case FMH::MODEL_KEY::DATE: {
            auto currentTime = QDateTime::currentDateTime();

            auto date1 = QDateTime::fromString(e1[key], Qt::TextDate);
            auto date2 = QDateTime::fromString(e2[key], Qt::TextDate);

            if (date1.secsTo(currentTime) < date2.secsTo(currentTime))
                return true;

            break;
        }
            
        case FMH::MODEL_KEY::MIME:
        case FMH::MODEL_KEY::LABEL: {
            const auto str1 = QString(e1[key]).toLower();
            const auto str2 = QString(e2[key]).toLower();

            if (str1 < str2)
                return true;
            break;
        }
            
        default:
            if (e1[key] < e2[key])
                return true;
        }
        
        return false;
    };

    if (this->foldersFirst) {
        
        it = std::partition(this->list.begin(),
                            this->list.end(),
                            [](const FMH::MODEL &e1) -> bool {
            return e1[FMH::MODEL_KEY::MIME] == QStringLiteral("inode/directory");
        });

        if(this->list.begin() != it)
        {
            std::sort(this->list.begin(), it, sortFunc);
        }
    }

    if(it != this->list.end())
    {
        std::sort(it, this->list.end(), sortFunc);
    }
}

QString FMList::getPathName() const
{
    return this->pathName;
}

QString FMList::getPath() const
{
    return this->path.toString();
}

void FMList::setPath(const QString &path)
{
    QUrl path_ = QUrl::fromUserInput(path.simplified(), QStringLiteral("/"), QUrl::AssumeLocalFile).adjusted(QUrl::PreferLocalFile | QUrl::StripTrailingSlash | QUrl::NormalizePathSegments);

    if (this->path == path_)
        return;

    this->path = path_;
    m_navHistory.appendPath(this->path);

    this->setStatus({PathStatus::STATUS_CODE::LOADING, i18n("Loading content"), i18n("Almost ready!"), QStringLiteral("view-refresh"), true, false});

    const auto __scheme = this->path.scheme();
    this->pathName = QDir(this->path.toLocalFile()).dirName();

    if (__scheme == FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::CLOUD_PATH]) {
        this->pathType = FMList::PATHTYPE::CLOUD_PATH;

    } else if (__scheme == FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::APPS_PATH]) {
        this->pathType = FMList::PATHTYPE::APPS_PATH;

    } else if (__scheme == FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::TAGS_PATH]) {
        this->pathType = FMList::PATHTYPE::TAGS_PATH;
        this->pathName = this->path.path();

    } else if (__scheme == FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::TRASH_PATH]) {
        this->pathType = FMList::PATHTYPE::TRASH_PATH;
        this->pathName = QStringLiteral("Trash");

    } else if (__scheme == FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::PLACES_PATH]) {
        this->pathType = FMList::PATHTYPE::PLACES_PATH;

    } else if (__scheme == FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::MTP_PATH]) {
        this->pathType = FMList::PATHTYPE::MTP_PATH;

    } else if (__scheme == FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::FISH_PATH]) {
        this->pathType = FMList::PATHTYPE::FISH_PATH;

    } else if (__scheme == FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::REMOTE_PATH]) {
        this->pathType = FMList::PATHTYPE::REMOTE_PATH;

    } else if (__scheme == FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::DRIVES_PATH]) {
        this->pathType = FMList::PATHTYPE::DRIVES_PATH;
    } else {
        this->pathType = FMList::PATHTYPE::OTHER_PATH;
    }

    Q_EMIT this->pathNameChanged();
    Q_EMIT this->pathTypeChanged();
    Q_EMIT this->pathChanged();
}

FMList::PATHTYPE FMList::getPathType() const
{
    return this->pathType;
}

QStringList FMList::getFilters() const
{
    return this->filters;
}

void FMList::setFilters(const QStringList &filters)
{
    if (this->filters == filters)
        return;

    this->filters = filters;

    Q_EMIT this->filtersChanged();
}

void FMList::resetFilters()
{
    this->setFilters(QStringList());
}

FMList::FILTER FMList::getFilterType() const
{
    return this->filterType;
}

void FMList::setFilterType(const FMList::FILTER &type)
{
    if (this->filterType == type)
        return;

    this->filterType = type;

    Q_EMIT this->filterTypeChanged();
}

void FMList::resetFilterType()
{
    this->setFilterType(FMList::FILTER::NONE);
}

bool FMList::getHidden() const
{
    return this->hidden;
}

void FMList::setHidden(const bool &state)
{
    if (this->hidden == state)
        return;

    this->hidden = state;
    Q_EMIT this->hiddenChanged();
}

bool FMList::getOnlyDirs() const
{
    return this->onlyDirs;
}

void FMList::setOnlyDirs(const bool &state)
{
    if (this->onlyDirs == state)
        return;

    this->onlyDirs = state;

    Q_EMIT this->onlyDirsChanged();
}

void FMList::refresh()
{
    Q_EMIT this->pathChanged();
}

void FMList::createDir(const QString &name)
{
    if(m_readOnly)
        return;
    
    if (this->pathType == FMList::PATHTYPE::CLOUD_PATH) {
#ifdef COMPONENT_SYNCING
        this->fm->createCloudDir(QString(this->path.toString()).replace(FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::CLOUD_PATH] + "/" + this->fm->sync->getUser(), ""), name);
#endif
    } else {
        FMStatic::createDir(this->path, name);
    }
}

void FMList::createFile(const QString& name)
{
    if(m_readOnly)
        return;
    
    FMStatic::createFile(this->path, name);
}

void FMList::renameFile(const QString& url, const QString& newName)
{
    if(m_readOnly)
        return;
    
    FMStatic::rename(QUrl(url), newName);
}

void FMList::moveToTrash(const QStringList& urls)
{
    if(m_readOnly)
        return;
    
    FMStatic::moveToTrash(QUrl::fromStringList(urls));
}

void FMList::removeFiles(const QStringList& urls)
{
    if(m_readOnly)
        return;
    
    FMStatic::removeFiles(QUrl::fromStringList(urls));
}

void FMList::createSymlink(const QString& url)
{
    if(m_readOnly)
        return;
    
    FMStatic::createSymlink(QUrl(url), this->path);
}

bool FMList::saveImageFile(const QImage& image)
{
    QString fileName = QString(QStringLiteral("%1/pasted_image-0.%2")).arg(path.toLocalFile(), QStringLiteral("png"));

    int idx = 1;
    while ( QFile::exists( fileName ) )
    {
        fileName = QString(QStringLiteral("%1/pasted_image-%2.%3")).arg(path.toLocalFile(), QString::number(idx), QStringLiteral("png"));
        idx++;
    }
    
    return image.save(fileName);
}

bool FMList::saveTextFile(const QString& data, const QString &format)
{
    QString fileName = QString(QStringLiteral("%1/pasted_text-0.%2")).arg(path.toLocalFile(), format);

    int idx = 1;
    while ( QFile::exists( fileName ) )
    {
        fileName = QString(QStringLiteral("%1/pasted_text-%2.%3")).arg(path.toLocalFile(), QString::number(idx), format);
        idx++;
    }
    
    QFile file(fileName);
    
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream out(&file);
        out << data;
        file.close();
        return true;
    }

    return false;
}

void FMList::paste()
{  
    if(m_readOnly)
        return;
    
    const QClipboard *clipboard = QGuiApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    
    if(!mimeData)
    {
        qWarning() << "Could not get mime data from the clipboard";
        return;
    }
    
    if (mimeData->hasImage())
    {
        saveImageFile(qvariant_cast<QImage>(mimeData->imageData()));
    } else if (mimeData->hasUrls())
    {
        const QByteArray a = mimeData->data(QStringLiteral("application/x-kde-cutselection"));
        const bool cut =  (!a.isEmpty() && a.at(0) == '1');

        if(cut)
        {
            cutInto(QUrl::toStringList(mimeData->urls()));
            // mimeData->clear();
        }else
        {
            copyInto(QUrl::toStringList(mimeData->urls()));
        }

    } else if (mimeData->hasText())
    {
        saveTextFile(mimeData->text(), QStringLiteral("txt"));
    } else
    {
        qWarning() << "Unexpected mime type from clipboard content for performing a paste";
    }
}

bool FMList::clipboardHasContent() const
{
    const QClipboard *clipboard = QGuiApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();
    
    if(!mimeData)
    {
        qWarning() << "Could not get mime data from the clipboard";
        return false;
    }
    
    return mimeData->hasUrls() || mimeData->hasImage() || mimeData->hasText();
}


void FMList::copyInto(const QStringList &urls)
{
    if(m_readOnly)
        return;
    
    this->fm->copy(QUrl::fromStringList(urls), this->path);
}

void FMList::cutInto(const QStringList &urls)
{
    if(m_readOnly)
        return;
    
    this->fm->cut(QUrl::fromStringList(urls), this->path);
}

void FMList::setDirIcon(const int &index, const QString &iconName)
{
    if (index >= this->list.size() || index < 0)
        return;

    const auto path = QUrl(this->list.at(index)[FMH::MODEL_KEY::PATH]);

    if (!FMStatic::isDir(path))
        return;

    FMStatic::setDirConf(QUrl(path.toString() + QStringLiteral("/.directory")), QStringLiteral("Desktop Entry"), QStringLiteral("Icon"), iconName);

    this->list[index][FMH::MODEL_KEY::ICON] = iconName;
    Q_EMIT this->updateModel(index, QVector<int> {FMH::MODEL_KEY::ICON});
}

const QUrl FMList::getParentPath()
{
    switch (this->pathType)
    {
    case FMList::PATHTYPE::PLACES_PATH:
        return FMStatic::parentDir(this->path);
    default:
        return this->previousPath();
    }
}

const QUrl FMList::posteriorPath()
{
    const auto url = m_navHistory.getPosteriorPath();

    if (url.isEmpty())
        return this->path;

    return url;
}

const QUrl FMList::previousPath()
{
    const auto url = m_navHistory.getPreviousPath();

    if (url.isEmpty())
        return this->path;

    return url;
}

bool FMList::getFoldersFirst() const
{
    return this->foldersFirst;
}

void FMList::setFoldersFirst(const bool &value)
{
    if (this->foldersFirst == value)
        return;

    Q_EMIT this->preListChanged();

    this->foldersFirst = value;

    Q_EMIT this->foldersFirstChanged();

    this->sortList();

    Q_EMIT this->postListChanged();
    Q_EMIT this->countChanged();
}

void FMList::componentComplete()
{
    connect(this, &FMList::pathChanged, this, &FMList::setList);
    connect(this, &FMList::filtersChanged, this, &FMList::setList);
    connect(this, &FMList::filterTypeChanged, this, &FMList::setList);
    connect(this, &FMList::hiddenChanged, this, &FMList::setList);
    connect(this, &FMList::onlyDirsChanged, this, &FMList::setList);
    
    connect(this, &FMList::sortByChanged, [this]()
    {
        if(this->list.size() > 0)
        {
            Q_EMIT this->preListChanged();
            this->sortList();
            Q_EMIT this->postListChanged();
            Q_EMIT this->countChanged();
        }
    });

    if(!this->path.isEmpty() && this->path.isValid())
    {
        this->setList();
    }
}

void FMList::search(const QString &query, bool recursive)
{
    if(this->path.isEmpty())
    {
        this->setStatus({PathStatus::ERROR, i18n("Error"), i18n("No path to perform the search"), QStringLiteral("document-info"), true, false});
    }

    qDebug() << "SEARCHING FOR" << query << this->path;

    if (!this->path.isLocalFile() || !recursive)
    { //if the path is not local then search will not be performed and instead will be filtered
        qWarning() << "URL recived is not a local file. So search will only filter the content" << this->path;
        this->filterContent(query, this->path);
        return;
    }

    QFutureWatcher<FMStatic::PATH_CONTENT> *watcher = new QFutureWatcher<FMStatic::PATH_CONTENT>;
    connect(watcher, &QFutureWatcher<FMH::MODEL_LIST>::finished, [=]() {
        const auto res = watcher->future().result();

        this->assignList(res.content);
        watcher->deleteLater();
    });

    QFuture<FMStatic::PATH_CONTENT> t1 = QtConcurrent::run([=]() -> FMStatic::PATH_CONTENT {
        FMStatic::PATH_CONTENT res;
        res.path = path;
        res.content = FMStatic::search(query, this->path, this->hidden, this->onlyDirs, this->filters);
        return res;
    });
    watcher->setFuture(t1);
}

void FMList::filterContent(const QString &query, const QUrl &path)
{
    if (this->list.isEmpty()) {
        qDebug() << "Can not filter content. List is empty";
        return;
    }

    QFutureWatcher<FMStatic::PATH_CONTENT> *watcher = new QFutureWatcher<FMStatic::PATH_CONTENT>;
    connect(watcher, &QFutureWatcher<FMH::MODEL_LIST>::finished, [=]() {
        const auto res = watcher->future().result();

        this->assignList(res.content);
        watcher->deleteLater();
    });

    QFuture<FMStatic::PATH_CONTENT> t1 = QtConcurrent::run([=]() -> FMStatic::PATH_CONTENT {
        FMH::MODEL_LIST m_content;
        FMStatic::PATH_CONTENT res;

        for (const auto &item : std::as_const(this->list))
        {
            if (item[FMH::MODEL_KEY::LABEL].contains(query, Qt::CaseInsensitive) || item[FMH::MODEL_KEY::SUFFIX].contains(query, Qt::CaseInsensitive) || item[FMH::MODEL_KEY::MIME].contains(query, Qt::CaseInsensitive)) {
                m_content << item;
            }
        }

        res.path = path;
        res.content = m_content;
        return res;
    });
    watcher->setFuture(t1);
}

int FMList::getCloudDepth() const
{
    return this->cloudDepth;
}

void FMList::setCloudDepth(const int &value)
{
    if (this->cloudDepth == value)
        return;

    this->cloudDepth = value;

    Q_EMIT this->cloudDepthChanged();
}

PathStatus FMList::getStatus() const
{
    return this->m_status;
}

void FMList::setStatus(const PathStatus &status)
{
    this->m_status = status;
    Q_EMIT this->statusChanged();
}

void FMList::remove(const int &index)
{
    if (index >= this->list.size() || index < 0)
        return;

    Q_EMIT this->preItemRemoved(index);
    this->list.remove(index);
    Q_EMIT this->postItemRemoved();
    Q_EMIT this->countChanged();
}

int FMList::indexOfName(const QString& query)
{
    const auto it = std::find_if(this->items().constBegin(), this->items().constEnd(), [query](const FMH::MODEL &item) -> bool {
        return item[FMH::MODEL_KEY::LABEL].startsWith(query, Qt::CaseInsensitive);
    });

    if (it != this->items().constEnd())
        return (std::distance(this->items().constBegin(), it));
    else
        return -1;
}

bool FMList::getAutoLoad() const
{
    return m_autoLoad;
}

void FMList::setAutoLoad(bool value)
{
    if(value == m_autoLoad)
    {
        return;
    }
    
    m_autoLoad = value;
    Q_EMIT autoLoadChanged();
}

bool FMList::readOnly() const
{
    return m_readOnly;
}

void FMList::setReadOnly(bool value)
{
    if(m_readOnly == value)
        return;
    
    m_readOnly = value;
    Q_EMIT readOnlyChanged();
}

int FMList::indexOfFile(const QString& url)
{
    const auto it = std::find_if(this->items().constBegin(), this->items().constEnd(), [url](const FMH::MODEL &item) -> bool {
        return item[FMH::MODEL_KEY::URL] == url;
    });
    
    if (it != this->items().constEnd())
        return (std::distance(this->items().constBegin(), it));
    else
        return -1;
}



