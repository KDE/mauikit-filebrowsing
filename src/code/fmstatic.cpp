#include "fmstatic.h"

#include <QDesktopServices>

#if defined Q_OS_LINUX && !defined Q_OS_ANDROID
#include <KCoreDirLister>
#include <KFileItem>
#include <KFilePlacesModel>
#include <KIO/CopyJob>
#include <KIO/DeleteJob>
#include <KIO/EmptyTrashJob>
#include <KIO/MkdirJob>
#include <KIO/SimpleJob>
#include <QIcon>
#endif

#ifdef COMPONENT_TAGGING
#include <MauiKit/FileTagging/tagging.h>
#endif

#include <MauiKit/platform.h>

#ifdef Q_OS_ANDROID
#include <MauiKit/mauiandroid.h>
#endif

FMStatic::FMStatic(QObject *parent)
    : QObject(parent)
{
}

FMH::MODEL_LIST FMStatic::packItems(const QStringList &items, const QString &type)
{
    FMH::MODEL_LIST data;

    for (const auto &path : items) {
        if (QUrl(path).isLocalFile() && !FMH::fileExists(path))
            continue;

        auto model = FMStatic::getFileInfoModel(path);
        model.insert(FMH::MODEL_KEY::TYPE, type);
        data << model;
    }

    return data;
}

FMH::MODEL_LIST FMStatic::getDefaultPaths()
{
    return FMStatic::packItems(FMH::defaultPaths, FMH::PATHTYPE_LABEL[FMH::PATHTYPE_KEY::PLACES_PATH]);
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
    drives << packItems(MAUIAndroid::sdDirs(), FMH::PATHTYPE_LABEL[FMH::PATHTYPE_KEY::DRIVES_PATH]);
    return drives;
#endif

    return drives;
}

bool FMStatic::isDefaultPath(const QString &path)
{
    return FMH::defaultPaths.contains(path);
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
    return path.scheme() == FMH::PATHTYPE_SCHEME[FMH::PATHTYPE_KEY::CLOUD_PATH];
}

bool FMStatic::fileExists(const QUrl &path)
{
    return FMH::fileExists(path);
}

QString FMStatic::fileDir(const QUrl &path) // the directory path of the file
{
    QString res = path.toString();
    if (path.isLocalFile()) {
        const QFileInfo file(path.toLocalFile());
        if (file.isDir())
            res = path.toString();
        else
            res = QUrl::fromLocalFile(file.dir().absolutePath()).toString();
    } else
        qWarning() << "The path is not a local one. FM::fileDir";
    
    return res;
}

QString FMStatic::homePath()
{
    return FMH::HomePath;
}
#if defined Q_OS_ANDROID || defined Q_OS_WIN32 || defined Q_OS_MACOS || defined Q_OS_IOS

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
#if defined Q_OS_ANDROID || defined Q_OS_WIN32 || defined Q_OS_MACOS || defined Q_OS_IOS
    for (const auto &url : urls) {
        QFileInfo srcFileInfo(url.toLocalFile());
        if (!srcFileInfo.isDir() && srcFileInfo.isFile()) {
            const auto _destination = QUrl(destinationDir.toString() + "/" + FMH::getFileInfoModel(url)[FMH::MODEL_KEY::LABEL]);
            if (!QFile::copy(url.toLocalFile(), _destination.toLocalFile())) {
                continue;
            }
        } else {
            const auto _destination = QUrl(destinationDir.toString() + "/" + FMH::getFileInfoModel(url)[FMH::MODEL_KEY::LABEL]);
            QDir destDir(_destination.toLocalFile());
            if (!destDir.exists())
                destDir.mkdir(_destination.toLocalFile());

            if (!copyRecursively(url.toLocalFile(), _destination.toLocalFile()))
                continue;
        }
    }
    return true;
#else
    auto job = KIO::copy(urls, destinationDir);
    job->start();
    return true;
#endif
}

bool FMStatic::cut(const QList<QUrl> &urls, const QUrl &where)
{
    return FMStatic::cut(urls, where, QString());
}

bool FMStatic::cut(const QList<QUrl> &urls, const QUrl &where, const QString &name)
{
#if defined Q_OS_ANDROID || defined Q_OS_WIN32 || defined Q_OS_MACOS || defined Q_OS_IOS
    for (const auto &url : qAsConst(urls)) {
        QUrl _where;
        if (name.isEmpty())
            _where = QUrl(where.toString() + "/" + FMH::getFileInfoModel(url)[FMH::MODEL_KEY::LABEL]);
        else
            _where = QUrl(where.toString() + "/" + name);

        QFile file(url.toLocalFile());
        file.rename(_where.toLocalFile());

#ifdef COMPONENT_TAGGING
        Tagging::getInstance()->updateUrl(url.toString(), _where.toString());
#endif
    }
#else
    QUrl _where = where;
    if (!name.isEmpty())
        _where = QUrl(where.toString() + "/" + name);

    auto job = KIO::move(urls, _where, KIO::HideProgressInfo);
    job->start();

#ifdef COMPONENT_TAGGING
    for (const auto &url : urls) {
        QUrl where_ = QUrl(where.toString() + "/" + FMStatic::getFileInfoModel(url)[FMH::MODEL_KEY::LABEL]);
        if (!name.isEmpty())
            where_ = QUrl(where.toString() + "/" + name);

        Tagging::getInstance()->updateUrl(url.toString(), where_.toString());
    }
#endif
#endif

    return true;
}

bool FMStatic::removeFiles(const QList<QUrl> &urls)
{
#ifdef COMPONENT_TAGGING
    for (const auto &url : qAsConst(urls)) {
        Tagging::getInstance()->removeUrl(url.toString());
    }
#endif

#if defined Q_OS_ANDROID || defined Q_OS_WIN32 || defined Q_OS_MACOS || defined Q_OS_IOS

    qDebug() << "ASKED GTO DELETE FILES" << urls;
    for (const auto &url : qAsConst(urls)) {
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
#else
    auto job = KIO::del(urls);
    job->start();
    return true;
#endif
}

void FMStatic::moveToTrash(const QList<QUrl> &urls)
{
#if defined Q_OS_LINUX && !defined Q_OS_ANDROID
    auto job = KIO::trash(urls);
    job->start();
#else
    Q_UNUSED(urls)
#endif
}

void FMStatic::emptyTrash()
{
#if defined Q_OS_LINUX && !defined Q_OS_ANDROID
    auto job = KIO::emptyTrash();
    job->start();
#endif
}

bool FMStatic::removeDir(const QUrl &path)
{
    bool result = true;
    QDir dir(path.toLocalFile());
    qDebug() << "TRYING TO REMOVE DIR" << path << path.toLocalFile();
    if (dir.exists()) {
        Q_FOREACH (QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::AllDirs | QDir::Files, QDir::DirsFirst)) {
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
    return FMStatic::cut({url}, QUrl(url.toString().left(url.toString().lastIndexOf("/"))), name);
}

bool FMStatic::createDir(const QUrl &path, const QString &name)
{
#if defined Q_OS_ANDROID || defined Q_OS_WIN32 || defined Q_OS_MACOS || defined Q_OS_IOS
    QFileInfo dd(path.toLocalFile());
    return QDir(path.toLocalFile()).mkdir(name);
#else
    auto job = KIO::mkdir(name.isEmpty() ? path : QUrl(path.toString() + "/" + name));
    job->start();
    return true;
#endif
}

bool FMStatic::createFile(const QUrl &path, const QString &name)
{
    QFile file(path.toLocalFile() + "/" + name);

    if (file.open(QIODevice::ReadWrite)) {
        file.close();
        return true;
    }

    return false;
}

bool FMStatic::createSymlink(const QUrl &path, const QUrl &where)
{
#if defined Q_OS_ANDROID || defined Q_OS_WIN32 || defined Q_OS_MACOS || defined Q_OS_IOS
    return QFile::link(path.toLocalFile(), where.toLocalFile() + "/" + QFileInfo(path.toLocalFile()).fileName());
#else
    qDebug() << "trying to create symlink" << path << where;
    const auto job = KIO::link({path}, where);
    job->start();
    return true;
#endif
}

bool FMStatic::openUrl(const QUrl &url)
{
    Platform::instance()->openUrl(url);
    //#ifdef Q_OS_ANDROID
    //    MAUIAndroid::openUrl(url.toString());
    //    return true;
    //#elif defined Q_OS_LINUX
    //    //     return QDesktopServices::openUrl(QUrl::fromUserInput(url));
    //    return KRun::runUrl(url, FMH::getFileInfoModel(url)[FMH::MODEL_KEY::MIME], nullptr, false, KRun::RunFlag::DeleteTemporaryFiles);
    //#elif defined Q_OS_WIN32 || defined Q_OS_MACOS || defined Q_OS_IOS
    //    return QDesktopServices::openUrl(url);
    //#endif
    return true;
}

void FMStatic::openLocation(const QStringList &urls)
{
    for (const auto &url : qAsConst(urls))
        QDesktopServices::openUrl(QUrl::fromLocalFile(QFileInfo(url).dir().absolutePath()));
}

const QVariantMap FMStatic::dirConf(const QUrl &path)
{
    return FMH::dirConf(path);
}

void FMStatic::setDirConf(const QUrl &path, const QString &group, const QString &key, const QVariant &value)
{
    FMH::setDirConf(path, group, key, value);
}

bool FMStatic::checkFileType(const int &type, const QString &mimeTypeName)
{
    return FMH::checkFileType(static_cast<FMH::FILTER_TYPE>(type), mimeTypeName);
}

void FMStatic::bookmark(const QUrl &url)
{
#if defined Q_OS_ANDROID || defined Q_OS_WIN32 || defined Q_OS_MACOS || defined Q_OS_IOS
    // do android stuff until cmake works with android
    if (isDefaultPath(url.toString()))
        return;

//     auto bookmarks = UTIL::loadSettings("BOOKMARKS", "PREFERENCES", {}, true).toStringList();
//     bookmarks << url.toString();
//     UTIL::saveSettings("BOOKMARKS", bookmarks, "PREFERENCES", true);
#else
    KFilePlacesModel model;
    model.addPlace(QDir(url.toLocalFile()).dirName(), url, FMH::getIconName(url));
#endif
}

QStringList FMStatic::nameFilters(const int &type)
{
    return FMH::FILTER_LIST[static_cast<FMH::FILTER_TYPE>(type)];
}

QString FMStatic::iconName(const QString &value)
{
    return FMH::getIconName(value);
}

static const QUrl thumbnailUrl(const QUrl &url, const QString &mimetype)
{
    #if defined Q_OS_LINUX && !defined Q_OS_ANDROID
    if (checkFileType(FMH::FILTER_TYPE::DOCUMENT, mimetype) || FMH::checkFileType(FMH::FILTER_TYPE::VIDEO, mimetype) || url.toString().endsWith(".appimage", Qt::CaseInsensitive)) {
        return QUrl("image://thumbnailer/" + url.toString());
    }
    #endif
    
    if (checkFileType(FMH::FILTER_TYPE::IMAGE, mimetype)) {
        return url;
    }
    
    return QUrl();
}

#if (!defined Q_OS_ANDROID && defined Q_OS_LINUX) || defined Q_OS_WIN
const FMH::MODEL FMStatic::getFileInfo(const KFileItem &kfile)
{
    return FMH::MODEL {{FMH::MODEL_KEY::LABEL, kfile.name()},
    {FMH::MODEL_KEY::NAME, kfile.name().remove(kfile.name().lastIndexOf("."), kfile.name().size())},
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
    {FMH::MODEL_KEY::WRITABLE, QVariant(kfile.isWritable()).toString()},
    {FMH::MODEL_KEY::READABLE, QVariant(kfile.isReadable()).toString()},
    {FMH::MODEL_KEY::EXECUTABLE, QVariant(kfile.isDesktopFile()).toString()},
    {FMH::MODEL_KEY::MIME, kfile.mimetype()},
    {FMH::MODEL_KEY::GROUP, kfile.group()},
    {FMH::MODEL_KEY::ICON, kfile.iconName()},
    {FMH::MODEL_KEY::SIZE, QString::number(kfile.size())},
    {FMH::MODEL_KEY::OWNER, kfile.user()},
    {FMH::MODEL_KEY::COUNT, kfile.isLocalFile() && kfile.isDir() ? QString::number(QDir(kfile.localPath()).count()) : "0"}};
}
#endif

const FMH::MODEL FMStatic::getFileInfoModel(const QUrl &path)
{
    FMH::MODEL res;
    #if defined Q_OS_ANDROID || defined Q_OS_MACOS || defined Q_OS_IOS || defined Q_OS_WIN
    const QFileInfo file(path.toLocalFile());
    if (!file.exists())
        return FMH::MODEL();
    
    const auto mime = FMH::getMime(path);
    res = FMH::MODEL {{FMH::MODEL_KEY::GROUP, file.group()},
    {FMH::MODEL_KEY::OWNER, file.owner()},
    {FMH::MODEL_KEY::SUFFIX, file.completeSuffix()},
    {FMH::MODEL_KEY::LABEL, /*file.isDir() ? file.baseName() :*/ path == HomePath ? QStringLiteral("Home") : file.fileName()},
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
    {FMH::MODEL_KEY::WRITABLE, QVariant(file.isWritable()).toString()},
    {FMH::MODEL_KEY::READABLE, QVariant(file.isReadable()).toString()},
    {FMH::MODEL_KEY::EXECUTABLE, QVariant(file.suffix().endsWith(".desktop")).toString()},
    {FMH::MODEL_KEY::ICON, getIconName(path)},
    {FMH::MODEL_KEY::SIZE, QString::number(file.size()) /*locale.formattedDataSize(file.size())*/},
    {FMH::MODEL_KEY::PATH, path.toString()},
    {FMH::MODEL_KEY::URL, path.toString()},
    {FMH::MODEL_KEY::THUMBNAIL, thumbnailUrl(path, mime).toString()},
    {FMH::MODEL_KEY::COUNT, file.isDir() ? QString::number(QDir(path.toLocalFile()).count()) : "0"}};
    #else
    res = getFileInfo(KFileItem(path, KFileItem::MimeTypeDetermination::NormalMimeTypeDetermination));
    #endif
    return res;
}

const QVariantMap FMStatic::getFileInfo(const QUrl &path)
{
    return FMH::toMap(getFileInfoModel(path));
}

const FMH::MODEL FMStatic::getDirInfoModel(const QUrl &path, const QString &type)
{
    auto res = getFileInfoModel(path);
    res[FMH::MODEL_KEY::TYPE] = type;
    return res;
}

const QVariantMap FMStatic::getDirInfo(const QUrl &path)
{
    return FMH::toMap(getDirInfoModel(path));
}
