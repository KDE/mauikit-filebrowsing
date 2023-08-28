#pragma once

#include <QtGlobal>
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <MauiKit3/Core/fmh.h>
#else
#include <MauiKit4/Core/fmh.h>
#endif

#include <QObject>
#include <QString>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>
#include <QStandardPaths>
#include <QUrl>

#ifdef Q_OS_ANDROID
#include <MauiKit3/Core/mauiandroid.h>
#endif

#ifdef KIO_AVAILABLE
class KFileItem;
#endif

#include "filebrowsing_export.h"

/**
 * @brief The FMStatic class
 * Static file management methods, this class has a constructor only to register to QML, however all methods are static.
 */
class FILEBROWSING_EXPORT FMStatic : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FMStatic)

public:
    explicit FMStatic(QObject *parent = nullptr);

    /**
     * @brief The FILTER_TYPE enum
     */
    enum FILTER_TYPE : int { AUDIO, VIDEO, TEXT, IMAGE, DOCUMENT, COMPRESSED, FONT, NONE };

    inline static const QStringList AUDIO_MIMETYPES = {
        QStringLiteral("audio/mpeg"),
        QStringLiteral("audio/mp4"),
        QStringLiteral("audio/flac"),
        QStringLiteral("audio/ogg"),
        QStringLiteral("audio/wav")};

    inline static const QStringList VIDEO_MIMETYPES = {
        QStringLiteral("video/mp4"),
        QStringLiteral("video/x-matroska"),
        QStringLiteral("video/webm"),
        QStringLiteral("video/avi"),
        QStringLiteral("video/flv"),
        QStringLiteral("video/mpg"),
        QStringLiteral("video/wmv"),
        QStringLiteral("video/mov"),
        QStringLiteral("video/quicktime"),
        QStringLiteral("video/ogg"),
        QStringLiteral("video/x-flv"),
        QStringLiteral("video/mpeg"),
        QStringLiteral("video/jpeg")};

    inline static const QStringList TEXT_MIMETYPES = {
        QStringLiteral("text/markdown"),
        QStringLiteral("text/x-chdr"),
        QStringLiteral("text/x-c++src"),
        QStringLiteral("text/x-c++hdr"),
        QStringLiteral("text/css"),
        QStringLiteral("text/html"),
        QStringLiteral("text/plain"),
        QStringLiteral("text/richtext"),
        QStringLiteral("text/scriptlet"),
        QStringLiteral("text/x-vcard"),
        QStringLiteral("text/x-go"),
        QStringLiteral("text/x-cmake"),
        QStringLiteral("text/x-makefile"),
        QStringLiteral("text/x-qml"),
        QStringLiteral("text/x-java"),
        QStringLiteral("text/x-log"),
        QStringLiteral("text/x-python"),
        QStringLiteral("text/*"),
        QStringLiteral("text/x-copying"),
        QStringLiteral("text/x-dbus-service"),
        QStringLiteral("text/x-gettext-translation"),
        QStringLiteral("application/xml"),
        QStringLiteral("application/vnd.kde.knotificationrc"),
        QStringLiteral("application/x-gitignore"),
        QStringLiteral("application/javascript"),
        QStringLiteral("application/json"),
        QStringLiteral("application/pgp-keys"),
        QStringLiteral("application/x-shellscript"),
        QStringLiteral("application/x-cmakecache"),
        QStringLiteral("application/x-yaml"),
        QStringLiteral("application/x-perl"),
        QStringLiteral("application/x-kdevelop"),
        QStringLiteral("application/x-kicad-project")};

    inline static const QStringList IMAGE_MIMETYPES = {
        QStringLiteral("image/bmp"),
        QStringLiteral("image/webp"),
        QStringLiteral("image/png"),
        QStringLiteral("image/gif"),
        QStringLiteral("image/jpeg"),
        QStringLiteral("image/web"),
        QStringLiteral("image/svg"),
        QStringLiteral("image/svg+xml"),
        QStringLiteral("application/x-krita"),
        QStringLiteral("image/x-xcf"),
        QStringLiteral("image/vnd.adobe.photoshop"),
        QStringLiteral("image/x-eps"),
        QStringLiteral("image/jxl"),
        QStringLiteral("image/avif")};

    inline static const QStringList DOCUMENT_MIMETYPES = {
        QStringLiteral("application/pdf"),
        QStringLiteral("application/rtf"),
        QStringLiteral("application/doc"),
        QStringLiteral("application/odf"),
        QStringLiteral("application/vnd.comicbook+zip"),
        QStringLiteral("application/vnd.comicbook+rar")};

    inline static const QStringList COMPRESSED_MIMETYPES = {
        QStringLiteral("application/x-compress"),
        QStringLiteral("application/x-compressed"),
        QStringLiteral("application/x-xz-compressed-tar"),
        QStringLiteral("application/x-compressed-tar"),
        //             "application/vnd.android.package-archive",
        QStringLiteral("application/x-xz"),
        QStringLiteral("application/x-bzip"),
        QStringLiteral("application/x-gtar"),
        QStringLiteral("application/x-gzip"),
        QStringLiteral("application/zip")};

    inline static const QStringList FONT_MIMETYPES = {
        QStringLiteral("font/ttf"),
        QStringLiteral("font/otf")};

    inline static const QMap<FILTER_TYPE, QStringList> SUPPORTED_MIMETYPES {{FILTER_TYPE::AUDIO, AUDIO_MIMETYPES},
                                                                            {FILTER_TYPE::VIDEO, VIDEO_MIMETYPES},
                                                                            {FILTER_TYPE::TEXT, TEXT_MIMETYPES},
                                                                            {FILTER_TYPE::IMAGE, IMAGE_MIMETYPES},
                                                                            {FILTER_TYPE::DOCUMENT, DOCUMENT_MIMETYPES},
                                                                            {FILTER_TYPE::FONT, FONT_MIMETYPES},
                                                                            {FILTER_TYPE::COMPRESSED, COMPRESSED_MIMETYPES}};

    /**
         * @brief getMimeTypeSuffixes
         * @param type
         * @return
         */
    inline static QStringList getMimeTypeSuffixes(const FILTER_TYPE &type, QString (*cb)(QString))
    {
        QStringList res;
        QMimeDatabase mimedb;
        for (const auto &mime : SUPPORTED_MIMETYPES[type]) {
            if (cb) {
                const auto suffixes = mimedb.mimeTypeForName(mime).suffixes();
                for (const QString &_suffix : suffixes) {
                    res << cb(_suffix);
                }
            } else {
                res << mimedb.mimeTypeForName(mime).suffixes();
            }
        }
        return res;
    }

    inline static auto func = [](QString suffix) -> QString {
        return QStringLiteral("*.") + suffix;
    };
    inline static QHash<FILTER_TYPE, QStringList> FILTER_LIST = {{FILTER_TYPE::AUDIO,
                                                                  getMimeTypeSuffixes(FILTER_TYPE::AUDIO,
                                                                  func)},
                                                                 {FILTER_TYPE::VIDEO,
                                                                  getMimeTypeSuffixes(FILTER_TYPE::VIDEO,
                                                                  func)},
                                                                 {FILTER_TYPE::TEXT,
                                                                  getMimeTypeSuffixes(FILTER_TYPE::TEXT,
                                                                  func)},
                                                                 {FILTER_TYPE::DOCUMENT,
                                                                  getMimeTypeSuffixes(FILTER_TYPE::DOCUMENT,
                                                                  func)},
                                                                 {FILTER_TYPE::COMPRESSED,
                                                                  getMimeTypeSuffixes(FILTER_TYPE::COMPRESSED,
                                                                  func)},
                                                                 {FILTER_TYPE::FONT,
                                                                  getMimeTypeSuffixes(FILTER_TYPE::FONT,
                                                                  func)},
                                                                 {FILTER_TYPE::IMAGE,
                                                                  getMimeTypeSuffixes(FILTER_TYPE::IMAGE,
                                                                  func)},
                                                                 {FILTER_TYPE::NONE, QStringList()}};

    /**
                                                                                                                                                                                 * @brief The PATH_CONTENT struct
                                                                                                                                                                                 */
    struct PATH_CONTENT {
        QUrl path; // the url holding all the content
        FMH::MODEL_LIST content; // the content from the url
    };

    /**
                                                                                                                                                                                 * @brief The PATHTYPE_KEY enum
                                                                                                                                                                                 */
    enum PATHTYPE_KEY : int {
        PLACES_PATH,
        REMOTE_PATH,
        DRIVES_PATH,
        REMOVABLE_PATH,
        TAGS_PATH,
        UNKNOWN_TYPE,
        APPS_PATH,
        TRASH_PATH,
        SEARCH_PATH,
        CLOUD_PATH,
        FISH_PATH,
        MTP_PATH,
        QUICK_PATH,
        BOOKMARKS_PATH,
        OTHER_PATH,
    };
    inline static const QHash<PATHTYPE_KEY, QString> PATHTYPE_SCHEME = {{PATHTYPE_KEY::PLACES_PATH, QStringLiteral("file")},
                                                                        {PATHTYPE_KEY::BOOKMARKS_PATH, QStringLiteral("file")},
                                                                        {PATHTYPE_KEY::DRIVES_PATH, QStringLiteral("drives")},
                                                                        {PATHTYPE_KEY::APPS_PATH, QStringLiteral("applications")},
                                                                        {PATHTYPE_KEY::REMOTE_PATH, QStringLiteral("remote")},
                                                                        {PATHTYPE_KEY::REMOVABLE_PATH, QStringLiteral("removable")},
                                                                        {PATHTYPE_KEY::UNKNOWN_TYPE, QStringLiteral("Unkown")},
                                                                        {PATHTYPE_KEY::TRASH_PATH, QStringLiteral("trash")},
                                                                        {PATHTYPE_KEY::TAGS_PATH, QStringLiteral("tags")},
                                                                        {PATHTYPE_KEY::SEARCH_PATH, QStringLiteral("search")},
                                                                        {PATHTYPE_KEY::CLOUD_PATH, QStringLiteral("cloud")},
                                                                        {PATHTYPE_KEY::FISH_PATH, QStringLiteral("fish")},
                                                                        {PATHTYPE_KEY::MTP_PATH, QStringLiteral("mtp")}};

    inline static const QHash<QString, PATHTYPE_KEY> PATHTYPE_SCHEME_NAME = {{PATHTYPE_SCHEME[PATHTYPE_KEY::PLACES_PATH], PATHTYPE_KEY::PLACES_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::BOOKMARKS_PATH], PATHTYPE_KEY::BOOKMARKS_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::DRIVES_PATH], PATHTYPE_KEY::DRIVES_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::APPS_PATH], PATHTYPE_KEY::APPS_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::REMOTE_PATH], PATHTYPE_KEY::REMOTE_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::REMOVABLE_PATH], PATHTYPE_KEY::REMOVABLE_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::UNKNOWN_TYPE], PATHTYPE_KEY::UNKNOWN_TYPE},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::TRASH_PATH], PATHTYPE_KEY::TRASH_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::TAGS_PATH], PATHTYPE_KEY::TAGS_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::SEARCH_PATH], PATHTYPE_KEY::SEARCH_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::CLOUD_PATH], PATHTYPE_KEY::CLOUD_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::FISH_PATH], PATHTYPE_KEY::FISH_PATH},
                                                                             {PATHTYPE_SCHEME[PATHTYPE_KEY::MTP_PATH], PATHTYPE_KEY::MTP_PATH}};

    inline static const QHash<PATHTYPE_KEY, QString> PATHTYPE_URI = {{PATHTYPE_KEY::PLACES_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::PLACES_PATH] + QStringLiteral("://")},
                                                                     {PATHTYPE_KEY::BOOKMARKS_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::BOOKMARKS_PATH] + QStringLiteral("://")},
                                                                     {PATHTYPE_KEY::DRIVES_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::DRIVES_PATH] + QStringLiteral("://")},
                                                                     {PATHTYPE_KEY::APPS_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::APPS_PATH] + QStringLiteral(":///")},
                                                                     {PATHTYPE_KEY::REMOTE_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::REMOTE_PATH] + QStringLiteral("://")},
                                                                     {PATHTYPE_KEY::REMOVABLE_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::REMOVABLE_PATH] + QStringLiteral("://")},
                                                                     {PATHTYPE_KEY::UNKNOWN_TYPE, PATHTYPE_SCHEME[PATHTYPE_KEY::UNKNOWN_TYPE] + QStringLiteral("://")},
                                                                     {PATHTYPE_KEY::TRASH_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::TRASH_PATH] + QStringLiteral("://")},
                                                                     {PATHTYPE_KEY::TAGS_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::TAGS_PATH] + QStringLiteral(":///")},
                                                                     {PATHTYPE_KEY::SEARCH_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::SEARCH_PATH] + QStringLiteral("://")},
                                                                     {PATHTYPE_KEY::CLOUD_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::CLOUD_PATH] + QStringLiteral(":///")},
                                                                     {PATHTYPE_KEY::FISH_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::FISH_PATH] + QStringLiteral("://")},
                                                                     {PATHTYPE_KEY::MTP_PATH, PATHTYPE_SCHEME[PATHTYPE_KEY::MTP_PATH] + QStringLiteral("://")}};


    /**
 *This is a user visible and translatable string, so it should not be used as a key anywhere

 **/
    static QString PathTypeLabel(const FMStatic::PATHTYPE_KEY &key);

    /**
                                                                                                                                                                                 * @brief DataPath
                                                                                                                                                                                 */
    inline static const QString DataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    /**
                                                                                                                                                                                 * @brief ConfigPath
                                                                                                                                                                                 */
    inline static const QString ConfigPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).toString();

    /**
                                                                                                                                                                                 * @brief CloudCachePath
                                                                                                                                                                                 */
    inline static const QString CloudCachePath = DataPath + QStringLiteral("/Cloud/");

    /**
                                                                                                                                                                                 * @brief DesktopPath
                                                                                                                                                                                 */
    inline static const QString DesktopPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();

    /**
                                                                                                                                                                                 * @brief AppsPath
                                                                                                                                                                                 */
    inline static const QString AppsPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)).toString();

    /**
                                                                                                                                                                                 * @brief RootPath
                                                                                                                                                                                 */
    inline static const QString RootPath = QUrl::fromLocalFile(QStringLiteral("/")).toString();


    inline static const QString PicturesPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();
    inline static const QString DownloadsPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).toString();
    inline static const QString DocumentsPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();
    inline static const QString MusicPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::MusicLocation)).toString();
    inline static const QString VideosPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)).toString();


#if defined(Q_OS_ANDROID)

    inline static const QString HomePath = QUrl::fromLocalFile( MAUIAndroid::homePath()).toString();

    inline static const QStringList defaultPaths = {HomePath, DocumentsPath, PicturesPath, MusicPath, VideosPath, DownloadsPath};

#else

    inline static const QString HomePath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();

    inline static const QString TrashPath = QStringLiteral("trash:/");

    inline static const QStringList defaultPaths = {
        HomePath,
        DesktopPath,
        DocumentsPath,
        DownloadsPath,
        MusicPath,
        PicturesPath,
        VideosPath,
        RootPath
    };

#endif

    inline static const QMap<QString, QString> folderIcon {{PicturesPath, QStringLiteral("folder-pictures")},
                                                           {DownloadsPath, QStringLiteral("folder-download")},
                                                           {DocumentsPath, QStringLiteral("folder-documents")},
                                                           {HomePath, QStringLiteral("user-home")},
                                                           {MusicPath, QStringLiteral("folder-music")},
                                                           {VideosPath, QStringLiteral("folder-videos")},
                                                           {DesktopPath, QStringLiteral("user-desktop")},
                                                           {AppsPath, QStringLiteral("system-run")},
                                                           {RootPath, QStringLiteral("folder-root")}};

public Q_SLOTS:
    /**
     * @brief search
     * Search for files in a path using filters
     * @param query
     * Term to be searched, such as ".qml" or "music"
     * @param path
     * The path to perform the search upon
     * @param hidden
     * If should also search for hidden files
     * @param onlyDirs
     * If only searching for directories and not files
     * @param filters
     * List of filter patterns such as {"*.qml"}, it can use regular expressions
     * @return
     * The search results are returned as a FMH::MODEL_LIST
     */
    static FMH::MODEL_LIST search(const QString &query, const QUrl &path, const bool &hidden = false, const bool &onlyDirs = false, const QStringList &filters = QStringList());

    /**
     * @brief getDevices
     * Devices mounted to the file system
     * @return
     * Represented as a FMH::MODEL_LIST
     */
    static FMH::MODEL_LIST getDevices();

    /**
     * @brief getDefaultPaths
     * A model list of the default paths in most systems, such as Home, Pictures, Video, Downloads, Music and Documents folders
     * @return
     */
    static FMH::MODEL_LIST getDefaultPaths();

    /**
     * @brief packItems
     * Given a list of path URLs pack all the info of such files as a FMH::MODEL_LIST
     * @param items
     * List of local URLs
     * @param type
     * The type of the list of urls, such as local, remote etc. This value is inserted with the key FMH::MODEL_KEY::TYPE
     * @return
     */
    static FMH::MODEL_LIST packItems(const QStringList &items, const QString &type);


    /**
     * @brief group
     * Perfom a move of the files to the passed destination
     * @param urls
     * List of URLs to be copy
     * @param destinationDir
     * Destination
     * @return
     * Return if the operation has been succesfull
     */
    static bool group(const QList<QUrl> &urls, const QUrl &destinationDir, const QString &name);

    /**
     * @brief copy
     * Perfom a copy of the files to the passed destination
     * @param urls
     * List of URLs to be copy
     * @param destinationDir
     * Destination
     * @return
     * Return if the operation has been succesfull
     */
    static bool copy(const QList<QUrl> &urls, const QUrl &destinationDir);

    /**
     * @brief cut
     * Perform a move/cut of a list of files to a destination. This function also moves the associated tags if the tags component has been enabled COMPONENT_TAGGING
     * @param urls
     * List of URLs to be moved
     * @param where
     * Destination path
     * @return
     * If the operation has been sucessfull
     */
    static bool cut(const QList<QUrl> &urls, const QUrl &where);

    /**
     * @brief cut
     * @param urls
     * @param where
     * @param name
     * New name of the files to be moved
     * @return
     */
    static bool cut(const QList<QUrl> &urls, const QUrl &where, const QString &name);

    /**
     * @brief removeFiles
     * List of files to be removed completely. This function also removes the assciated tags to the files if the tagging component has been enabled COMPONENT_TAGGING
     * @param urls
     * @return
     * If the operation has been sucessfull
     */
    static bool removeFiles(const QList<QUrl> &urls);

    /**
     * @brief removeDir
     * Remove a directory recursively
     * @param path
     * Path URL to be rmeoved
     * @return
     * If the operation has been sucessfull
     */
    static bool removeDir(const QUrl &path);

    /**
     * @brief homePath
     * The default home path in different systems
     * @return
     */
    static QString homePath();

    /**
     * @brief parentDir
     * Given a file url return its parent directory
     * @param path
     * The file URL
     * @return
     * The parent directory URL if it exists otherwise returns the passed URL
     */
    static QUrl parentDir(const QUrl &path);

    /**
     * @brief isDefaultPath
     * Checks if a given path URL is a default path as in returned by the defaultPaths method
     * @param path
     * @return
     */
    static bool isDefaultPath(const QString &path);

    /**
     * @brief isDir
     * If a local file URL is a directory
     * @param path
     * File URL
     * @return
     */
    static bool isDir(const QUrl &path);

    /**
     * @brief isCloud
     * If a path is a URL server instead of a local file
     * @param path
     * @return
     */
    static bool isCloud(const QUrl &path);

    /**
     * @brief fileExists
     * Checks if a local file exists in the file system
     * @param path
     * File URL
     * @return
     * Existance
     */
    static bool fileExists(const QUrl &path);

    /**
     * if the url is a file path then it returns its directory
     * and if it is a directory returns the same path
     * */
    /**
     * @brief fileDir
     * Gives the directory URL path of a file,  and if it is a directory returns the same path
     * @param path
     * File path URL
     * @return
     * The directory URL
     */
    static QUrl fileDir(const QUrl &path);

    //     /* SETTINGS */
    //     /**
    //      * @brief saveSettings
    //      * Saves a key-value settings into a config file determined by the application name and org. This is a interface to the Utils utility that takes care of resolving the right path for the config file.
    //      * @param key
    //      * Key of the setting
    //      * @param value
    //      * Value fo the key setting
    //      * @param group
    //      * The group to which the key belongs
    //      */
    //     static void saveSettings(const QString &key, const QVariant &value, const QString &group);

    //     /**
    //      * @brief loadSettings
    //      * Loads a setting value
    //      * @param key
    //      * Setting key
    //      * @param group
    //      * Setting group
    //      * @param defaultValue
    //      * In case the setting key is not present use this default value
    //      * @return
    //      * A QVariant holding the setting value
    //      */
    //     static QVariant loadSettings(const QString &key, const QString &group, const QVariant &defaultValue);


    /**
     * @brief setDirConf
     * Write a config key-value to the directory config file
     * @param path
     * @param group
     * @param key
     * @param value
     */
    static void setDirConf(const QUrl &path, const QString &group, const QString &key, const QVariant &value);

    /**
     * @brief checkFileType
     * Checks if a mimetype belongs to a file type, for example image/jpg belong to the type FMH::FILTER_TYPE
     * @param type
     * FMH::FILTER_TYPE value
     * @param mimeTypeName
     * @return
     */
    static bool checkFileType(const int &type, const QString &mimeTypeName);
    static bool checkFileType(const FMStatic::FILTER_TYPE &type, const QString &mimeTypeName);

    /**
     * @brief moveToTrash
     * Moves to the trash can the file URLs. The associated tags are kept in case the files are restored.
     * @param urls
     */
    static void moveToTrash(const QList<QUrl> &urls);

    /**
     * @brief emptyTrash
     * Empty the trash casn
     */
    static void emptyTrash();

    /**
     * @brief rename
     * Rename a file to a new name
     * @param url
     * File URL to be renamed
     * @param name
     * The short new name of the file, not the new URL, for setting a new URl use cut instead.
     * @return
     */
    static bool rename(const QUrl &url, const QString &name);

    /**
     * @brief createDir
     * Creates a directory given a base path and a directory name
     * @param path
     * Base directory path
     * @param name
     * New directory name
     * @return
     * If the operation was sucessfull
     */
    static bool createDir(const QUrl &path, const QString &name);

    /**
     * @brief createFile
     * Creates a file given the base directory path and a short file name
     * @param path
     * Base directory path
     * @param name
     * Name of the new file to be created with the extension
     * @return
     */
    static bool createFile(const QUrl &path, const QString &name);

    /**
     * @brief createSymlink
     * Creates a symlink
     * @param path
     * File to be symlinked
     * @param where
     * Destination of the symlink
     * @return
     */
    static bool createSymlink(const QUrl &path, const QUrl &where);

    /**
     * @brief openUrl
     * Given a URL it tries to open it using the default app associated to it
     * @param url
     * The URL to be open
     * @return
     */
    static void openUrl(const QUrl &url);

    /**
     * @brief openLocation
     * Open with the default file manager a list of URLs
     * @param urls
     */
    static void openLocation(const QStringList &urls);

    /**
     * @brief bookmark
     * Add a URL to the places bookmarks
     * @param url
     * The file URL to be bookmarked
     */
    static void bookmark(const QUrl &url);

    /**
     * @brief nameFilters
     * Given a filter type return a list of associated name filters, as in suffixes.
     * @param type
     * The filter type to be mapped to a FMH::FILTER_TYPE
     */
    static QStringList nameFilters(const int &type);

    /**
     * @brief iconName
     * Get the icon name associated to the file or name.
     * @param value
     * The file path or file name
     */
    static QString iconName(const QString &value);

#ifdef KIO_AVAILABLE
    /**
     * @brief getFileInfo
     * Get file info
     * @param path
     * @return
     * File info packed as a QVariantMap model
     */
    static const FMH::MODEL getFileInfo(const KFileItem &kfile);
#endif

    /**
     * @brief getFileInfoModel
     * @param path
     * @return
     */
    static const FMH::MODEL getFileInfoModel(const QUrl &path);

    /**
     * @brief getFileInfo
     * @param path
     * @return
     */
    static const QVariantMap getFileInfo(const QUrl &path);

    /**
     * @brief getIconName
     * Returns the icon name for certain file.
     * The file path must be represented as a local file URL.
     * It also looks into the directory config file to get custom set icons
     * @param path
     * @return
     */
    static const QString getIconName(const QUrl &path);

    /**
     * Return icon name set in the directory .directory file
     * The passed path must be a local file URL.
     **/
    /**
     * @brief dirConf
     * @param path
     * @return
     */
    static const QString dirConfIcon(const QUrl &path);

    /**
     * @brief getMime
     * @param path
     * @return
     */
    static const QString getMime(const QUrl &path);

    /**
     * @brief getPathType
     * @param url
     * @return
     */
    static FMStatic::PATHTYPE_KEY getPathType(const QUrl &url);
};
