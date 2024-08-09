#pragma once

#include <QtGlobal>

#include <MauiKit4/Core/fmh.h>

#ifdef Q_OS_ANDROID
#include <MauiKit4/Core/mauiandroid.h>
#endif

#include <QObject>
#include <QString>
#include <QMimeData>
#include <QMimeDatabase>
#include <QMimeType>
#include <QStandardPaths>
#include <QUrl>
#include <QQmlEngine>

#ifdef KIO_AVAILABLE
class KFileItem;
#endif

#include "filebrowsing_export.h"

/**
 * @brief The FMStatic class is a group of static file management methods, this class has a constructor only as a way to register it to QML, however all methods in here  are static.
 */
class FILEBROWSING_EXPORT FMStatic : public QObject
{
    Q_OBJECT
    // QML_NAMED_ELEMENT(FM)
    // QML_SINGLETON

    Q_DISABLE_COPY_MOVE(FMStatic)

public:
    /**
     * @private
     */
    explicit FMStatic(QObject *parent = nullptr);

    /**
     * @brief The common file types for filtering
     */
    enum FILTER_TYPE : int { AUDIO, VIDEO, TEXT, IMAGE, DOCUMENT, COMPRESSED, FONT, NONE };

    /**
     * @brief The list of supported audio formats, associated to `FILTER_TYPE::AUDIO`
     */
    inline static const QStringList AUDIO_MIMETYPES = {
        QStringLiteral("audio/mpeg"),
        QStringLiteral("audio/mp4"),
        QStringLiteral("audio/flac"),
        QStringLiteral("audio/ogg"),
        QStringLiteral("audio/wav")};

    /**
     * @brief The list of supported video formats, associated to `FILTER_TYPE::VIDEO`
     */
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

    /**
     * @brief The list of supported text formats, associated to `FILTER_TYPE::TEXT`
     */
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
        QStringLiteral("application/yaml"),
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

    /**
     * @brief The list of supported image formats, associated to `FILTER_TYPE::IMAGE`
     */
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

    /**
     * @brief The list of supported document formats, associated to `FILTER_TYPE::DOCUMENT`
     */
    inline static const QStringList DOCUMENT_MIMETYPES = {
        QStringLiteral("application/pdf"),
        QStringLiteral("application/rtf"),
        QStringLiteral("application/doc"),
        QStringLiteral("application/odf"),
        QStringLiteral("application/vnd.comicbook+zip"),
        QStringLiteral("application/vnd.comicbook+rar")};

    /**
     * @brief The list of supported archive formats, associated to `FILTER_TYPE::COMPRESSED`
     */
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

    /**
     * @brief The list of supported font formats, associated to `FILTER_TYPE::FONT`
     */
    inline static const QStringList FONT_MIMETYPES = {
        QStringLiteral("font/ttf"),
        QStringLiteral("font/otf")};

    /**
         * @brief The map set of the supported mime types for the FM classes. This structure maps the `FILTER_TYPE` to the associated list of mime types.
         *
         * For example `SUPPORTED_MIMETYPES[FILTER_TYPE::AUDIO]` would return a list of mimetypes associated to the FILTER_TYPE::AUDIO, such as `"audio/mpeg", "audio/mp4", "audio/flac", "audio/ogg", "audio/wav"`.
         */
    inline static const QMap<FILTER_TYPE, QStringList> SUPPORTED_MIMETYPES {{FILTER_TYPE::AUDIO, AUDIO_MIMETYPES},
                                                                            {FILTER_TYPE::VIDEO, VIDEO_MIMETYPES},
                                                                            {FILTER_TYPE::TEXT, TEXT_MIMETYPES},
                                                                            {FILTER_TYPE::IMAGE, IMAGE_MIMETYPES},
                                                                            {FILTER_TYPE::DOCUMENT, DOCUMENT_MIMETYPES},
                                                                            {FILTER_TYPE::FONT, FONT_MIMETYPES},
                                                                            {FILTER_TYPE::COMPRESSED, COMPRESSED_MIMETYPES}};

    /**
         * @brief Given a FILTER_TYPE and its associated mime-types, return a list of all the supported file extension suffixes.
         * @param type the FILTER_TYPE
         * @param cb a callback function to modify the gathered suffix extension. This function will receive the supported suffix and it can return a new string of a modified suffix or the same one. This is optional.
         * @return the list of associated file extensions
         */
    inline static QStringList getMimeTypeSuffixes(const FILTER_TYPE &type, QString (*cb)(QString) = nullptr)
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

    /**
     * @private
     */
    inline static auto func = [](QString suffix) -> QString {
        return QStringLiteral("*.") + suffix;
    };

    /**
     * @brief Convenient map set of file type extensions.
     * The values make use of the regex wildcard operator [*] meant for filtering a directory contents, for example.
     * `FILTER_LIST[FILTER_TYPE::AUDIO]` could possible return something alike `["*.mp3", "*.mp4", "*.mpeg", "*.wav"]` etc.
     */
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

    /**                                                                                                                                                                                 * @brief A location contents structured for convenience.                                                                                                                                                                                 */
    struct PATH_CONTENT {

        /**
         * @brief The location URL holding all of the contents.
         */
        QUrl path;

        /**
         * @brief The contents of the location.
         */
        FMH::MODEL_LIST content;
    };

    /**                                                                                                                                                                                 * @brief The different location types supported. Most of them need of KDE KIO framework to be fully operational.                                                                                                                                                                                */
    enum PATHTYPE_KEY : int {
        /**
         * Local paths, such as the Downloads, Pictures, etc. `file:/`
         */
        PLACES_PATH,

        /**
         * Remote locations, such as servers accessed via SSH or FTP
         */
        REMOTE_PATH,

        /**
         * Hard drives locations
         */
        DRIVES_PATH,

        /**
         * Removable places, such as optic CDs, USB pen drives, etc.
         */
        REMOVABLE_PATH,

        /**
         * A tag location.
         */
        TAGS_PATH,

        /**
         * Unknown location type.
         */
        UNKNOWN_TYPE,

        /**
         * The applications location. Accessed with KIO via the `applications://` scheme.
         */
        APPS_PATH,

        /**
         * The trash location. `trash:/`
         */
        TRASH_PATH,

        /**
         * A search results.
         */
        SEARCH_PATH,

        /**
         * A remote cloud server path.
         */
        CLOUD_PATH,

        /**
         * A remote SHH or FTP. `fish:/` `ftp:/`
         */
        FISH_PATH,

        /**
         * MTP path
         */
        MTP_PATH,

        /**
         * The common standard paths
         */
        QUICK_PATH,

        /**
         * A bookmarked location. `file:/`
         */
        BOOKMARKS_PATH,

        /**
         * Any other path
         */
        OTHER_PATH,
    };

    /**
     * @brief The map of the PATH_TYPE to its associated protocol scheme.
     * For example `PATHTYPE_SCHEME[PATHTYPE_KEY::TRASH_PATH] = "trash"`, `PATHTYPE_SCHEME[PATHTYPE_KEY::PLACES_PATH] = "file"`
     */
    inline static const QHash<PATHTYPE_KEY, QString> PATHTYPE_SCHEME = {{PATHTYPE_KEY::PLACES_PATH, QStringLiteral("file")},
                                                                        {PATHTYPE_KEY::BOOKMARKS_PATH, QStringLiteral("file")},
                                                                        {PATHTYPE_KEY::DRIVES_PATH, QStringLiteral("drives")},
                                                                        {PATHTYPE_KEY::APPS_PATH, QStringLiteral("applications")},
                                                                        {PATHTYPE_KEY::REMOTE_PATH, QStringLiteral("remote")},
                                                                        {PATHTYPE_KEY::REMOVABLE_PATH, QStringLiteral("removable")},
                                                                        {PATHTYPE_KEY::UNKNOWN_TYPE, QStringLiteral("unknown")},
                                                                        {PATHTYPE_KEY::TRASH_PATH, QStringLiteral("trash")},
                                                                        {PATHTYPE_KEY::TAGS_PATH, QStringLiteral("tags")},
                                                                        {PATHTYPE_KEY::SEARCH_PATH, QStringLiteral("search")},
                                                                        {PATHTYPE_KEY::CLOUD_PATH, QStringLiteral("cloud")},
                                                                        {PATHTYPE_KEY::FISH_PATH, QStringLiteral("fish")},
                                                                        {PATHTYPE_KEY::MTP_PATH, QStringLiteral("mtp")}};

    /**
    * @brief The protocol scheme mapped to its PATHTYPE_KEY.
    * Where `PATHTYPE_SCHEME_NAME["file"] = FMH::PATHTYPE_KEY::PLACES_PATH`
    */
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

    /**
    * @brief Similar to PATHTYPE_SCHEME, but mapped with the complete scheme.
    *  For example `PATHTYPE_URIE[PATHTYPE_KEY::TRASH_PATH] = "trash://"`, `PATHTYPE_URI[PLACES_PATH] = "file://"`
    */
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
 * @brief Given a PATHTYPE_KEY return a user friendly string.
 * @warning This is a user visible and translatable string, so it should not be used as a key anywhere
 * @param key the PATHTYPE_KEY key
 **/
    static QString PathTypeLabel(const FMStatic::PATHTYPE_KEY &key);

    /**                                                                                                                                                                                 * @brief Standard Data location path                                                                                                                                                                               */
    inline static const QString DataPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);

    /**                                                                                                                                                                                 * @brief Standard Configuration location path                                                                                                                                                                                 */
    inline static const QString ConfigPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)).toString();

    /**                                                                                                                                                                                * @brief Standard Cloud Cache location path                                                                                                                                                                                 */
    inline static const QString CloudCachePath = DataPath + QStringLiteral("/Cloud/");

    /**                                                                                                                                                                                 * @brief Standard desktop location path                                                                                                                                                                                 */
    inline static const QString DesktopPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation)).toString();

    /**                                                                                                                                                                                 * @brief Standard applications location path                                                                                                                                                                                 */
    inline static const QString AppsPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation)).toString();

    /**                                                                                                                                                                                 * @brief Standard root location path                                                                                                                                                                                */
    inline static const QString RootPath = QUrl::fromLocalFile(QStringLiteral("/")).toString();

    /**                                                                                                                                                                                 * @brief Standard pictures location path                                                                                                                                                                                */
    inline static const QString PicturesPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)).toString();

    /**                                                                                                                                                                                 * @brief Standard downloads location path                                                                                                                                                                                */
    inline static const QString DownloadsPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation)).toString();

    /**                                                                                                                                                                                 * @brief Standard documents location path                                                                                                                                                                                */
    inline static const QString DocumentsPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)).toString();

    /**                                                                                                                                                                                 * @brief Standard music location path                                                                                                                                                                                */
    inline static const QString MusicPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::MusicLocation)).toString();

    /**                                                                                                                                                                                 * @brief Standard videos location path                                                                                                                                                                                */
    inline static const QString VideosPath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::MoviesLocation)).toString();


#if defined(Q_OS_ANDROID)
    /**                                                                                                                                                                                 * @brief Standard home location path                                                                                                                                                                                */
    inline static const QString HomePath = QUrl::fromLocalFile( MAUIAndroid::homePath()).toString();

    /**                                                                                                                                                                                 * @brief The internally defined quick standard locations.                                                                                                                                                                    */
    inline static const QStringList defaultPaths = {HomePath, DocumentsPath, PicturesPath, MusicPath, VideosPath, DownloadsPath};
#else

    /**                                                                                                                                                                                 * @brief Standard home location path                                                                                                                                                                                */
    inline static const QString HomePath = QUrl::fromLocalFile(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)).toString();

    /**                                                                                                                                                                                 * @brief Standard trash location path                                                                                                                                                                                */
    inline static const QString TrashPath = QStringLiteral("trash:/");

    /**                                                                                                                                                                                 * @brief The internally defined quick standard locations.                                                                                                                                                                    */
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

    /**
     * @brief A mapping of the standard location to a icon name.
     */
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
     * @brief Search for files in a path using name filters
     * @param query the term query to be searched, such as `".qml"` or `"music"`
     * @param path the path where to perform or start the search
     * @param hidden whether to search for hidden files
     * @param onlyDirs whether to only search for directories and not files
     * @param filters list of filter patterns such as `{"*.qml"}`, it can use regular expressions.
     * @return the search results are returned as a FMH::MODEL_LIST
     */
    static FMH::MODEL_LIST search(const QString &query, const QUrl &path, const bool &hidden = false, const bool &onlyDirs = false, const QStringList &filters = QStringList());

    /**
     * @brief Devices mounted in the file system
     * @return list of devices represented as a FMH::MODEL_LIST with information
     */
    static FMH::MODEL_LIST getDevices();

    /**
     * @brief A model list of the default paths in most systems, such as Home, Pictures, Video, Downloads, Music and Documents folders
     * @return the packaged model with information for each directory
     */
    static FMH::MODEL_LIST getDefaultPaths();

    /**
     * @brief Given a list of path URLs pack all the info of such files as a FMH::MODEL_LIST
     * @param items list of local URLs
     * @param type the type of the list of URLs, such as local, remote etc. This value is inserted with the key `FMH::MODEL_KEY::TYPE`
     * @return
     */
    static FMH::MODEL_LIST packItems(const QStringList &items, const QString &type);

    /**
     * @brief Perform a move operation of the given files to a new destination
     * @param urls list of URLs to be copied
     * @param destinationDir destination
     * @param name the name of the new directory where all the entries will be grouped/moved into
     * @return whether the operation has been successful
     */
    static bool group(const QList<QUrl> &urls, const QUrl &destinationDir, const QString &name);

    /**
     * @brief Perform a copy of the files to the given destination
     * @param urls list of URLs to be copy
     * @param destinationDir destination
     * @return whether the operation has been successful
     */
    static bool copy(const QList<QUrl> &urls, const QUrl &destinationDir);

    /**
     * @brief Perform a move/cut of a list of files to a destination. This function also moves the associated tags.
     * @param urls list of URLs to be moved
     * @param where destination path
     * @return whether  the operation has been successful
     */
    static bool cut(const QList<QUrl> &urls, const QUrl &where);

    /**
     * @see cut
     * @param name new name of the directory where the files will be pasted
     */
    static bool cut(const QList<QUrl> &urls, const QUrl &where, const QString &name);

    /**
     * @brief List of files to be removed completely. This function also removes the associated tags to the files.
     * @param urls file URLs to be removed
     * @return Whether the operation has been sucessfull
     */
    static bool removeFiles(const QList<QUrl> &urls);

    /**
     * @brief Remove a directory recursively.
     * @param path directory URL to be removed
     * @return whether the operation has been sucessfull
     */
    static bool removeDir(const QUrl &path);

    /**
     * @brief The default home path.
     * @return URL of the home location
     */
    static QString homePath();

    /**
     * @brief Given a file URL return its parent directory
     * @param path the file URL
     * @return the parent directory URL if it exists otherwise returns the passed URL
     */
    static QUrl parentDir(const QUrl &path);

    /**
     * @brief Checks if a given path URL is a default path as found in the `defaultPaths` method
     * @param path the directory location path
     * @return whether is a default path
     */
    static bool isDefaultPath(const QString &path);

    /**
     * @brief Whether a local file URL is a directory
     * @param path file URL
     * @return is a directory
     */
    static bool isDir(const QUrl &path);

    /**
     * @brief Whether a path is a URL server instead of a local file
     * @param path
     * @return
     */
    static bool isCloud(const QUrl &path);

    /**
     * @brief Checks if a local file exists in the file system
     * @param path file URL
     * @return whether the file path exists locally
     */
    static bool fileExists(const QUrl &path);

    /**
     * @brief Given a file URL, return its parent directory
     * @note If the given path is a directory then returns the same path.
     * @param path file path URL
     * @return the directory URL
     */
    static QUrl fileDir(const QUrl &path);

    /**
     * @brief Write a configuration key-value entry to the directory conf file
     * @param path directory path
     * @param group the entry group name
     * @param key the key name of the entry
     * @param value the value of the entry
     */
    static void setDirConf(const QUrl &path, const QString &group, const QString &key, const QVariant &value);

    /**
     * @brief Checks if a mime-type belongs to a file type, for example, whether `image/jpg` belongs to the type `FMH::FILTER_TYPE`
     * @param type FMH::FILTER_TYPE value
     * @param mimeTypeName the mime type name
     * @return whether the type contains the given name
     */
    static bool checkFileType(const int &type, const QString &mimeTypeName);
    static bool checkFileType(const FMStatic::FILTER_TYPE &type, const QString &mimeTypeName);

    /**
     * @brief Moves to the trashcan the provided file URLs. The associated tags are kept in case the files are restored.
     * @param urls the file URLs
     */
    static void moveToTrash(const QList<QUrl> &urls);

    /**
     * @brief Empty the trashcan
     */
    static void emptyTrash();

    /**
     * @brief Rename a file. The associated tags will be updated.
     * @param url the file URL to be renamed
     * @param name the new name of the file, not the new URL, for setting a new URl use cut instead.
     * @return whether the operation was successful.
     */
    static bool rename(const QUrl &url, const QString &name);

    /**
     * @brief Creates a new directory given a base path and a name
     * @param path base path where to create the new directory
     * @param name the new directory name
     * @return whether the operation was successful
     */
    static bool createDir(const QUrl &path, const QString &name);

    /**
     * @brief Creates a file given the base directory path and a file name
     * @param path the base location path
     * @param name the name of the new file to be created with the extension
     * @return whether the operation was successful
     */
    static bool createFile(const QUrl &path, const QString &name);

    /**
     * @brief Creates a symbolic link to a given file URL
     * @param path file to be linked
     * @param where destination of the symbolic link
     * @return whether the operation was successful
     */
    static bool createSymlink(const QUrl &path, const QUrl &where);

    /**
     * @brief Given a URL it tries to open it using the default application associated to it
     * @param url the file URL to be opened
     */
    static void openUrl(const QUrl &url);

    /**
     * @brief Open the file URLs with the default file manager
     * @param urls file or location URLs to be opened
     */
    static void openLocation(const QStringList &urls);

    /**
     * @brief Add a directory URL to the places bookmarks
     * @param url the directory URL to be bookmarked
     */
    static void bookmark(const QUrl &url);

    /**
     * @brief Given a filter type return a list of associated name filters, as their suffixes.
     * @param type the filter type to be mapped to a FMH::FILTER_TYPE
     * @see FMH::FILTER_LIST
     */
    static QStringList nameFilters(const int &type);

#ifdef KIO_AVAILABLE
    /**
     * @brief Get the file information packaged in a key-value model.
     * @param path the file path
     * @return the file info packed into a model
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
     * @brief Returns the icon name for certain file.
     * The file path must be represented as a local file URL.
     * It also looks into the directory conf file to get the directory custom icon.
     *
     * @note To get an abstract icon, use a template name, such as `test.jpg`, to get an icon for the JPG image type. The file does not need to exists.
     * @param path file path
     * @return
     */
    static const QString getIconName(const QUrl &path);

    /**
     * @brief Return the icon name set in the directory `.directory` conf file.
     * The passed path must be a local file URL.
     * @param path the directory location
     * @return the icon name
     */
    static const QString dirConfIcon(const QUrl &path);

    /**
     * @brief Get the mime type of the given file path
     * @param path the file path
     * @return the mime-type string
     */
    static const QString getMime(const QUrl &path);

    /**
     * @brief Given a file URL with a well defined scheme, get the PATHTYPE_KEY.
     * @see PATHTYPE_KEY
     * @param url the file URL
     * @return the file PATHTYPE_KEY
     */
    static FMStatic::PATHTYPE_KEY getPathType(const QUrl &url);
};
