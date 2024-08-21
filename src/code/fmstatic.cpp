#include "fmstatic.h"
#include "placeslist.h"
#include "tagging.h"

#include <QDesktopServices>
#include <QDirIterator>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QSettings>

#include <KLocalizedString>

#if (defined Q_OS_LINUX || defined Q_OS_FREEBSD) && !defined Q_OS_ANDROID 
#include <KConfig>
#include <KConfigGroup>
#endif

#ifdef KIO_AVAILABLE
#include <KIO/CopyJob>
#include <KIO/DeleteJob>
#include <KIO/EmptyTrashJob>
#include <KIO/MkdirJob>
#include <KIO/SimpleJob>
#include <KIO/OpenUrlJob>
#include <KApplicationTrader>
#include <KIO/PreviewJob>
#include <KFileItem>
#endif

QString FMStatic::PathTypeLabel(const FMStatic::PATHTYPE_KEY& key)
{
    static const QHash<FMStatic::PATHTYPE_KEY, QString> PATHTYPE_LABEL = {{PATHTYPE_KEY::PLACES_PATH, i18n("Places")},
                                                                          {PATHTYPE_KEY::BOOKMARKS_PATH, i18n("Bookmarks")},
                                                                          {PATHTYPE_KEY::DRIVES_PATH, i18n("Drives")},
                                                                          {PATHTYPE_KEY::APPS_PATH, i18n("Apps")},
                                                                          {PATHTYPE_KEY::REMOTE_PATH, i18n("Remote")},
                                                                          {PATHTYPE_KEY::REMOVABLE_PATH, i18n("Removable")},
                                                                          {PATHTYPE_KEY::UNKNOWN_TYPE, i18n("Unknown")},
                                                                          {PATHTYPE_KEY::TRASH_PATH, i18n("Trash")},
                                                                          {PATHTYPE_KEY::TAGS_PATH, i18n("Tags")},
                                                                          {PATHTYPE_KEY::SEARCH_PATH, i18n("Search")},
                                                                          {PATHTYPE_KEY::CLOUD_PATH, i18n("Cloud")},
                                                                          {PATHTYPE_KEY::FISH_PATH, i18n("Remote")},
                                                                          {PATHTYPE_KEY::MTP_PATH, i18n("Drives")},
                                                                          {PATHTYPE_KEY::OTHER_PATH, i18n("Others")},
                                                                          {PATHTYPE_KEY::QUICK_PATH, i18n("Quick")}};

    return PATHTYPE_LABEL[key];
}


FMStatic::FMStatic(QObject *parent)
    : QObject(parent)
{}

FMH::MODEL_LIST FMStatic::packItems(const QStringList &items, const QString &type)
{
    FMH::MODEL_LIST data;

    for (const auto &path : items)
    {
        if (QUrl(path).isLocalFile() && !FMH::fileExists(QUrl(path)))
            continue;

        auto model = FMStatic::getFileInfoModel(QUrl(path));
        model.insert(FMH::MODEL_KEY::TYPE, type);
        data << model;
    }

    return data;
}

FMH::MODEL_LIST FMStatic::getDefaultPaths()
{
    return FMStatic::packItems(FMStatic::defaultPaths, PathTypeLabel(FMStatic::PATHTYPE_KEY::PLACES_PATH));
}

FMH::MODEL_LIST FMStatic::search(const QString &query, const QUrl &path, const bool &hidden, const bool &onlyDirs, const QStringList &filters)
{
    FMH::MODEL_LIST content;

    if (!path.isLocalFile()) {
        qWarning() << "URL recived is not a local file. FM::search" << path;
        return content;
    }

    if (FMStatic::isDir(path)) {
        QDir::Filters dirFilter;

        dirFilter = (onlyDirs ? QDir::AllDirs | QDir::NoDotDot | QDir::NoDot : QDir::Files | QDir::AllDirs | QDir::NoDotDot | QDir::NoDot);

        if (hidden)
            dirFilter = dirFilter | QDir::Hidden | QDir::System;

        QDirIterator it(path.toLocalFile(), filters, dirFilter, QDirIterator::Subdirectories);
        while (it.hasNext()) {
            auto url = it.next();
            if (it.fileName().contains(query, Qt::CaseInsensitive)) {
                content << FMStatic::getFileInfoModel(QUrl::fromLocalFile(url));
            }
        }
    } else
        qWarning() << "Search path does not exists" << path;

    qDebug() << content;
    return content;
}

FMH::MODEL_LIST FMStatic::getDevices()
{
    FMH::MODEL_LIST drives;

#ifdef Q_OS_ANDROID
    drives << packItems(MAUIAndroid::sdDirs(), PathTypeLabel(FMStatic::PATHTYPE_KEY::DRIVES_PATH));
    return drives;
#endif

    return drives;
}

bool FMStatic::isDefaultPath(const QString &path)
{
    return FMStatic::defaultPaths.contains(path);
}

QUrl FMStatic::parentDir(const QUrl &path)
{
    if (!path.isLocalFile()) {
        qWarning() << "URL recived is not a local file, FM::parentDir" << path;
        return path;
    }
    
    QDir dir(path.toLocalFile());
    dir.cdUp();
    return QUrl::fromLocalFile(dir.absolutePath());
}

bool FMStatic::isDir(const QUrl &path)
{
    if (!path.isLocalFile()) {
        //         qWarning() << "URL recived is not a local file. FM::isDir" << path;
        return false;
    }

    const QFileInfo file(path.toLocalFile());
    return file.isDir();
}

bool FMStatic::isCloud(const QUrl &path)
{
    return path.scheme() == FMStatic::PATHTYPE_SCHEME[FMStatic::PATHTYPE_KEY::CLOUD_PATH];
}

bool FMStatic::fileExists(const QUrl &path)
{
    return FMH::fileExists(path);
}

QUrl FMStatic::fileDir(const QUrl &path) // the directory path of the file
{
    QUrl res = path;
    if (path.isLocalFile()) {
        const QFileInfo file(path.toLocalFile());
        if (file.isDir())
            res = path;
        else
            res = QUrl::fromLocalFile(file.dir().absolutePath());
    } else
        qWarning() << "The path is not a local one. FM::fileDir";
    
    return res;
}

QString FMStatic::homePath()
{
    return FMStatic::HomePath;
}
#if !defined KIO_AVAILABLE

static bool copyRecursively(QString sourceFolder, QString destFolder)
{
    bool success = false;
    QDir sourceDir(sourceFolder);

    if (!sourceDir.exists())
        return false;

    QDir destDir(destFolder);
    if (!destDir.exists())
        destDir.mkdir(destFolder);

    QStringList files = sourceDir.entryList(QDir::Files);
    for (int i = 0; i < files.count(); i++) {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        success = QFile::copy(srcName, destName);
        if (!success)
            return false;
    }

    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < files.count(); i++) {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        success = copyRecursively(srcName, destName);
        if (!success)
            return false;
    }

    return true;
}
#endif

bool FMStatic::copy(const QList<QUrl> &urls, const QUrl &destinationDir)
{
#ifdef KIO_AVAILABLE
    auto job = KIO::copy(urls, destinationDir);
    job->start();
    return true;
#else
    for (const auto &url : std::as_const(urls)) {
        QFileInfo srcFileInfo(url.toLocalFile());
        if (!srcFileInfo.isDir() && srcFileInfo.isFile()) {
            const auto _destination = QUrl(destinationDir.toString() + QStringLiteral("/") + FMStatic::getFileInfoModel(url)[FMH::MODEL_KEY::LABEL]);
            if (!QFile::copy(url.toLocalFile(), _destination.toLocalFile())) {
                continue;
            }
        } else {
            const auto _destination = QUrl(destinationDir.toString() + QStringLiteral("/") + FMStatic::getFileInfoModel(url)[FMH::MODEL_KEY::LABEL]);
            QDir destDir(_destination.toLocalFile());
            if (!destDir.exists())
                destDir.mkdir(_destination.toLocalFile());

            if (!copyRecursively(url.toLocalFile(), _destination.toLocalFile()))
                continue;
        }
    }
    return true;
#endif
}

bool FMStatic::group(const QList<QUrl>& urls, const QUrl& destinationDir, const QString &name)
{
    if(!FMStatic::fileExists(QUrl(destinationDir.toString() + QStringLiteral("/") + name)))
    {
        QDir(destinationDir.toLocalFile()).mkdir(name);
    }
    
    return FMStatic::cut(urls, destinationDir, name);
}


bool FMStatic::cut(const QList<QUrl> &urls, const QUrl &where)
{
    return FMStatic::cut(urls, where, QString());
}

bool FMStatic::cut(const QList<QUrl> &urls, const QUrl &where, const QString &name)
{
#ifdef KIO_AVAILABLE
    QUrl _where = where;
    if (!name.isEmpty())
        _where = QUrl(where.toString() + QStringLiteral("/") + name);
    
    auto job = KIO::move(urls, _where, KIO::HideProgressInfo);
    job->start();

    for (const auto &url : urls) {
        QUrl where_ = QUrl(where.toString() + QStringLiteral("/") + FMStatic::getFileInfoModel(url)[FMH::MODEL_KEY::LABEL]);
        if (!name.isEmpty())
            where_ = QUrl(where.toString() + QStringLiteral("/") + name);

        Tagging::getInstance()->updateUrl(url.toString(), where_.toString());
    }
#else
    for (const auto &url : std::as_const(urls)) {
        QUrl _where;
        if (name.isEmpty())
            _where = QUrl(where.toString() + QStringLiteral("/") + FMStatic::getFileInfoModel(url)[FMH::MODEL_KEY::LABEL]);
        else
            _where = QUrl(where.toString() + QStringLiteral("/") + name);

        QFile file(url.toLocalFile());
        file.rename(_where.toLocalFile());

        Tagging::getInstance()->updateUrl(url.toString(), _where.toString());
    }
#endif

    return true;
}

bool FMStatic::removeFiles(const QList<QUrl> &urls)
{
    for (const auto &url : std::as_const(urls)) {
        Tagging::getInstance()->removeUrl(url.toString());
    }

#ifdef KIO_AVAILABLE
    auto job = KIO::del(urls);
    job->start();
    return true;
#else
    qDebug() << "ASKED GTO DELETE FILES" << urls;
    for (const auto &url : std::as_const(urls)) {
        qDebug() << "@ Want to remove files << " << url.toLocalFile();

        if (isDir(url)) {
            qDebug() << "Want to remove dir << " << url.toLocalFile();
            FMStatic::removeDir(url);
        } else {
            qDebug() << "Want to remove files << " << url.toLocalFile();
            QFile::remove(url.toLocalFile());
        }
    }
    return true;
#endif
}

void FMStatic::moveToTrash(const QList<QUrl> &urls)
{
#ifdef KIO_AVAILABLE
    auto job = KIO::trash(urls);
    job->start();
#else
    Q_UNUSED(urls)
#endif
}

void FMStatic::emptyTrash()
{
#ifdef KIO_AVAILABLE
    auto job = KIO::emptyTrash();
    job->start();
#endif
}

bool FMStatic::removeDir(const QUrl &path)
{
    bool result = true;
    QDir dir(path.toLocalFile());
    qDebug() << "TRYING TO REMOVE DIR" << path << path.toLocalFile();
    if (dir.exists())
    {
        for (const QFileInfo &info : dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if (info.isDir()) {
                result = removeDir(QUrl::fromLocalFile(info.absoluteFilePath()));
            } else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(path.toLocalFile());
    }

    return result;
}

bool FMStatic::rename(const QUrl &url, const QString &name)
{
    return FMStatic::cut({url}, QUrl(url.toString().left(url.toString().lastIndexOf(QStringLiteral("/")))), name);
}

bool FMStatic::createDir(const QUrl &path, const QString &name)
{
#ifdef KIO_AVAILABLE
    auto job = KIO::mkdir(name.isEmpty() ? path : QUrl(path.toString() + QStringLiteral("/") + name));
    job->start();
    return true;
#else
    return QDir(path.toLocalFile()).mkdir(name);
#endif
}

bool FMStatic::createFile(const QUrl &path, const QString &name)
{
    QFile file(path.toLocalFile() + QStringLiteral("/") + name);

    if (file.open(QIODevice::ReadWrite)) {
        file.close();
        return true;
    }

    return false;
}

bool FMStatic::createSymlink(const QUrl &path, const QUrl &where)
{
#ifdef KIO_AVAILABLE
    qDebug() << "trying to create symlink" << path << where;
    const auto job = KIO::link({path}, where);
    job->start();
    return true;
#else
    return QFile::link(path.toLocalFile(), where.toLocalFile() + QStringLiteral("/") + QFileInfo(path.toLocalFile()).fileName());
#endif
}

void FMStatic::openUrl(const QUrl &url)
{
#ifdef KIO_AVAILABLE
    KIO::OpenUrlJob *job = new KIO::OpenUrlJob(url);
    job->setRunExecutables(true);
    job->start();
#else

#if defined Q_OS_ANDROID
    MAUIAndroid::openUrl(url);
#else
    QDesktopServices::openUrl(url);
#endif
    
#endif    
}

void FMStatic::openLocation(const QStringList &urls)
{
    for (const auto &url : std::as_const(urls))
    {
        if(isDir(QUrl(url)))
        {
            QDesktopServices::openUrl(QUrl(url));
        }else
        {
            QDesktopServices::openUrl(fileDir(QUrl(url)));
        }
    }
}

const QString FMStatic::dirConfIcon(const QUrl &path)
{
    QString icon = QStringLiteral("folder");
    
    if (!path.isLocalFile()) {
        qWarning() << "URL recived is not a local file" << path;
        return icon;
    }
    
    if (!fileExists(path))
        return icon;
    
    
#if defined Q_OS_ANDROID || defined Q_OS_WIN || defined Q_OS_MACOS || defined Q_OS_IOS
    QSettings file(path.toLocalFile(), QSettings::Format::NativeFormat);
    file.beginGroup(QStringLiteral("Desktop Entry"));
    icon = file.value(QStringLiteral("Icon"), icon).toString();
    file.endGroup();
#else
    KConfig file(path.toLocalFile());
    const auto map = file.entryMap(QStringLiteral("Desktop Entry"));
    icon = map.isEmpty() ? QStringLiteral("folder") : map.value(QStringLiteral("Icon"));
#endif
    
    return icon;
}

void FMStatic::setDirConf(const QUrl &path, const QString &group, const QString &key, const QVariant &value)
{
    if (!path.isLocalFile()) {
        qWarning() << "URL recived is not a local file" << path;
        return;
    }
    
#if defined Q_OS_ANDROID || defined Q_OS_WIN || defined Q_OS_MACOS || defined Q_OS_IOS
    QSettings file(path.toLocalFile(), QSettings::Format::IniFormat);
    file.beginGroup(group);
    file.setValue(key, value);
    file.endGroup();
    file.sync();
#else
    KConfig file(path.toLocalFile(), KConfig::SimpleConfig);
    auto kgroup = file.group(group);
    kgroup.writeEntry(key, value);
    // 		file.reparseConfiguration();
    file.sync();
#endif
}

bool FMStatic::checkFileType(const int &type, const QString &mimeTypeName)
{
    return FMStatic::checkFileType(static_cast<FMStatic::FILTER_TYPE>(type), mimeTypeName);
}

bool FMStatic::checkFileType(const FMStatic::FILTER_TYPE &type, const QString &mimeTypeName)
{
    return SUPPORTED_MIMETYPES[type].contains(mimeTypeName);
}

void FMStatic::bookmark(const QUrl &url)
{
    PlacesList::addBookmark(url);
}

QStringList FMStatic::nameFilters(const int &type)
{
    return FMStatic::FILTER_LIST[static_cast<FMStatic::FILTER_TYPE>(type)];
}

const QString FMStatic::getMime(const QUrl &path)
{
    if (!path.isLocalFile()) {
        qWarning() << "URL recived is not a local file, getMime" << path;
        return QString();
    }
    
    const QMimeDatabase mimedb;
    return mimedb.mimeTypeForFile(path.toLocalFile()).name();
}

static const QUrl thumbnailUrl(const QUrl &url, const QString &mimetype)
{
#ifdef KIO_AVAILABLE
    if (FMStatic::checkFileType(FMStatic::FILTER_TYPE::FONT, mimetype) || FMStatic::checkFileType(FMStatic::FILTER_TYPE::TEXT, mimetype) || FMStatic::checkFileType(FMStatic::FILTER_TYPE::AUDIO, mimetype) || FMStatic::checkFileType(FMStatic::FILTER_TYPE::DOCUMENT, mimetype) || FMStatic::checkFileType(FMStatic::FILTER_TYPE::VIDEO, mimetype) || url.toString().endsWith(QStringLiteral(".appimage"), Qt::CaseInsensitive)) {
        return QUrl(QStringLiteral("image://thumbnailer/") + url.toString());
    }
    
    //      if (KIO::PreviewJob::supportedMimeTypes().contains(mimetype)) {
    //         return QUrl("image://thumbnailer/" + url.toString());
    //     }
#endif
    
    if (FMStatic::checkFileType(FMStatic::FILTER_TYPE::IMAGE, mimetype)) {
        return url;
    }
    
    return QUrl();
}

#ifdef KIO_AVAILABLE
const FMH::MODEL FMStatic::getFileInfo(const KFileItem &kfile)
{
    return FMH::MODEL {
        {FMH::MODEL_KEY::LABEL, kfile.name()},
        {FMH::MODEL_KEY::NAME, kfile.name()},
        {FMH::MODEL_KEY::DATE, kfile.time(KFileItem::FileTimes::CreationTime).toString(Qt::TextDate)},
        {FMH::MODEL_KEY::MODIFIED, kfile.time(KFileItem::FileTimes::ModificationTime).toString(Qt::TextDate)},
        {FMH::MODEL_KEY::LAST_READ, kfile.time(KFileItem::FileTimes::AccessTime).toString(Qt::TextDate)},
        {FMH::MODEL_KEY::PATH, kfile.mostLocalUrl().toString()},
        {FMH::MODEL_KEY::URL, kfile.mostLocalUrl().toString()},
        {FMH::MODEL_KEY::THUMBNAIL, thumbnailUrl(kfile.mostLocalUrl(), kfile.mimetype()).toString()},
        {FMH::MODEL_KEY::SYMLINK, kfile.linkDest()},
        {FMH::MODEL_KEY::IS_SYMLINK, QVariant(kfile.isLink()).toString()},
        {FMH::MODEL_KEY::HIDDEN, QVariant(kfile.isHidden()).toString()},
        {FMH::MODEL_KEY::IS_DIR, QVariant(kfile.isDir()).toString()},
        {FMH::MODEL_KEY::IS_FILE, QVariant(kfile.isFile()).toString()},
        {FMH::MODEL_KEY::MIME, kfile.mimetype()},
        {FMH::MODEL_KEY::ICON, kfile.iconName()},
        {FMH::MODEL_KEY::SIZE, QString::number(kfile.size())},
        {FMH::MODEL_KEY::COUNT, kfile.isLocalFile() && kfile.isDir() ? QString::number(QDir(kfile.localPath()).count()-2) : QStringLiteral("0")}};
        }
#endif

        const FMH::MODEL FMStatic::getFileInfoModel(const QUrl &path)
        {
            FMH::MODEL res;
#ifdef KIO_AVAILABLE
            res = FMStatic::getFileInfo(KFileItem(path, KFileItem::MimeTypeDetermination::NormalMimeTypeDetermination));
#else
            if(FMStatic::getPathType(path) == FMStatic::PATHTYPE_KEY::TAGS_PATH)
            {
                res = FMH::MODEL {
                {FMH::MODEL_KEY::LABEL, path.fileName()},
                {FMH::MODEL_KEY::NAME, path.fileName()},
                {FMH::MODEL_KEY::IS_SYMLINK, QStringLiteral("false")},
                {FMH::MODEL_KEY::IS_FILE, QStringLiteral("false")},
                {FMH::MODEL_KEY::HIDDEN, QStringLiteral("false")},
                {FMH::MODEL_KEY::IS_DIR, QStringLiteral("false")},
                {FMH::MODEL_KEY::SIZE, QStringLiteral("0")}, /*locale.formattedDataSize(file.size())*/
                {FMH::MODEL_KEY::PATH, path.toString()},
                {FMH::MODEL_KEY::URL, path.toString()}};

            }else
            {
                const QFileInfo file(path.toLocalFile());
                if (!file.exists())
                    return FMH::MODEL();

                const auto mime = FMStatic::getMime(path);
                res = FMH::MODEL {
                {FMH::MODEL_KEY::SUFFIX, file.completeSuffix()},
                {FMH::MODEL_KEY::LABEL, path.toString() == HomePath ? QStringLiteral("Home") : file.fileName()},
                {FMH::MODEL_KEY::NAME, file.fileName()},
                {FMH::MODEL_KEY::DATE, file.birthTime().toString(Qt::TextDate)},
                {FMH::MODEL_KEY::MODIFIED, file.lastModified().toString(Qt::TextDate)},
                {FMH::MODEL_KEY::LAST_READ, file.lastRead().toString(Qt::TextDate)},
                {FMH::MODEL_KEY::MIME, mime},
                {FMH::MODEL_KEY::SYMLINK, file.symLinkTarget()},
                {FMH::MODEL_KEY::IS_SYMLINK, QVariant(file.isSymLink()).toString()},
                {FMH::MODEL_KEY::IS_FILE, QVariant(file.isFile()).toString()},
                {FMH::MODEL_KEY::HIDDEN, QVariant(file.isHidden()).toString()},
                {FMH::MODEL_KEY::IS_DIR, QVariant(file.isDir()).toString()},
                {FMH::MODEL_KEY::SIZE, QString::number(file.size()) /*locale.formattedDataSize(file.size())*/},
                {FMH::MODEL_KEY::PATH, path.toString()},
                {FMH::MODEL_KEY::URL, path.toString()},
                {FMH::MODEL_KEY::THUMBNAIL, thumbnailUrl(path, mime).toString()},
                {FMH::MODEL_KEY::COUNT, file.isDir() ? QString::number(QDir(path.toLocalFile()).count()) : QStringLiteral("0")}};
        }
        #endif


                        res[FMH::MODEL_KEY::ICON] = getIconName(path); //to control what icon is shown
                        return res;
            }


                        const QVariantMap FMStatic::getFileInfo(const QUrl &path)
                {
                        return FMH::toMap(getFileInfoModel(path));
            }

                        const QString FMStatic::getIconName(const QUrl &path)
                {
                        if(FMStatic::getPathType(path) == FMStatic::PATHTYPE_KEY::TAGS_PATH)
                {
                        return QStringLiteral("tag");
            }

                        if (path.isLocalFile() && QFileInfo(path.toLocalFile()).isDir())
                {
                        if (folderIcon.contains(path.toString()))
                        return folderIcon[path.toString()];
                        else
                {
                        return dirConfIcon(QUrl(QString(path.toString() + QStringLiteral("/%1")).arg(QStringLiteral(".directory"))));
            }

            } else {
        #ifdef KIO_AVAILABLE
                        KFileItem mime(path);
                        return mime.iconName();
        #else
                        QMimeDatabase mime;
                        const auto type = mime.mimeTypeForFile(path.toString());
                        return type.iconName();
        #endif
            }
            }

            FMStatic::PATHTYPE_KEY FMStatic::getPathType(const QUrl &url)
            {
                return FMStatic::PATHTYPE_SCHEME_NAME[url.scheme()];
            }
