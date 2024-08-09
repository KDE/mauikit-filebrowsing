#pragma once

#include <QObject>

#include <QStringList>
#include <QVariantList>
#include <QDirIterator>
#include <QVector>

#include <MauiKit4/Core/fmh.h>

#include "fmstatic.h"
#include "filebrowsing_export.h"

#ifdef KIO_AVAILABLE
class KCoreDirLister;
#else

class QFileSystemWatcher;

namespace FMH
{
class FileLoader;
}
/**
 * @private The QDirLister class is a placeholder for the KCoreDirLister for other systems other than GNU Linux.
 */
class QDirLister : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QDirLister)
    
public:
    explicit QDirLister(QObject *parent = nullptr);

public Q_SLOTS:
    /**
     * @brief openUrl
     * @param url
     * @return
     */
    bool openUrl(const QUrl &url);

    /**
     * @brief setNameFilter
     * @param filters
     */
    void setNameFilter(QString filters);

    /**
     * @brief setDirOnlyMode
     * @param value
     */
    void setDirOnlyMode(bool value);

    /**
     * @brief setShowingDotFiles
     * @param value
     */
    void setShowingDotFiles(bool value);

    /**
     * @brief setShowHiddenFiles Placeholder method, just calls back to setShowingDotFiles
     * @param value
     */
    void setShowHiddenFiles(bool value);

Q_SIGNALS:
    void itemsReady(FMH::MODEL_LIST items, QUrl url);
    void itemReady(FMH::MODEL item, QUrl url);
    void completed(QUrl url);
    void itemsAdded(FMH::MODEL_LIST items, QUrl url);
    void itemsDeleted(FMH::MODEL_LIST items, QUrl url);
    void newItems(FMH::MODEL_LIST items, QUrl url);
    void refreshItems(QVector<QPair<FMH::MODEL, FMH::MODEL>> items, QUrl url);

private:
    FMH::FileLoader *m_loader;
    QFileSystemWatcher *m_watcher;

    FMH::MODEL_LIST m_list;
    QString m_nameFilters;
    QUrl m_url;
    bool m_dirOnly = false;
    bool m_showDotFiles = false;

    bool m_checking = false;

    void reviewChanges();
    bool includes(const QUrl &url);
    int indexOf(const FMH::MODEL_KEY &key, const QString &value) const;
};
#endif

class Syncing;

/**
 * @brief The FM class stands for File Management, and exposes methods for file listing, browsing and handling, with syncing and tagging integration if such components were enabled with the build flags `COMPONENT_SYNCING` and `COMPONENT_TAGGING`.
 * 
 * @warning File syncing support with webDAV cloud providers, such as NextCloud, is still work in progress.  
 * 
 */
class FILEBROWSING_EXPORT FM : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(FM)
    
public:
    
    /**
     * @brief Creates the instance.
     */
    FM(QObject *parent = nullptr);

    //Syncing
    
    /**
     * @brief Given a server URL address retrieve its contents. This only works if the syncing component has been enabled with `COMPONENT_SYNCING`
     * @see cloudServerContentReady
     * @param server the server URL
     * @param filters the list of string filters to be applied
     * @param depth how deep in the directory three to go, for example, `1` keeps the retrieval in the first level or current directory.
     * @return whether the operation was successful. 
     */
    bool getCloudServerContent(const QUrl &server, const QStringList &filters = QStringList(), const int &depth = 0);

    /**
     * @brief Creates a directory in the server. This only works if the syncing component has been enabled `COMPONENT_SYNCING`.
     * @param path the server location where to create the new directory
     * @param name directory name
     */
    Q_INVOKABLE void createCloudDir(const QString &path, const QString &name);

    /**
     * @brief Given a path URL retrieve the contents information packaged as a model. This method is asynchronous and once items become ready the signals will be emitted, such as, `pathContentItemsReady` or `pathContentReady`
     * @param path the directory path
     * @param hidden whether to pack hidden files
     * @param onlyDirs whether to only pack directories
     * @param filters the list of string filters to be applied
     * @param iteratorFlags the directory iterator flags, for reference check QDirIterator documentation
     */
    void getPathContent(const QUrl &path, const bool &hidden = false, const bool &onlyDirs = false, const QStringList &filters = QStringList(), const QDirIterator::IteratorFlags &iteratorFlags = QDirIterator::NoIteratorFlags);

    /**
     * @brief Given a remote server address URL, resolve it to the local cache URL. This only works if the syncing component has been enabled `COMPONENT_SYNCING=ON`
     * @param path the remote Server address
     * @return the resolved server path as a local URL
     */
    QString resolveLocalCloudPath(const QString &path);

    /**
     * @brief Given the server address and the user name, resolve a local path for the cache of the files.
     * @param server the remove server address
     * @param user the user name of the server
     * @return
     */
    static QString resolveUserCloudCachePath(const QString &server, const QString &user);

#ifdef COMPONENT_SYNCING
    Syncing *sync;
#endif

private:

#ifdef KIO_AVAILABLE
    KCoreDirLister *dirLister;
#else
    QDirLister *dirLister;
#endif

Q_SIGNALS:
    
    /**
     * @brief Emitted once the requested contents of the server are ready.
     * @param list the contents packaged in a list, with the file information 
     */
    void cloudServerContentReady(FMStatic::PATH_CONTENT list);
    
    /**
     * @brief Emitted for every single item that becomes available, from the requested remote server location.
     * @param item the item data
     * @param path the URL of the remote location initially requested
     */
    void cloudItemReady(FMH::MODEL item, QUrl path); 

    /**
     * @brief Emitted once the contents of the current location are ready and the listing has finished.
     * @param path the requested location path
     */
    void pathContentReady(QUrl path);
    
    /**
     * @brief Emitted when a set of entries for the current location are ready.
     * @param list the contents packaged in a list, with the file information 
     */
    void pathContentItemsReady(FMStatic::PATH_CONTENT list);
    
    /**
     * @brief Emitted when the contents of the current location has changed, either by some new entries being added or removed.
     * @param path the requested location which has changed
     */
    void pathContentChanged(QUrl path);
    
    /**
     * @brief Emitted when the current location entries have changed.
     * @param items the list of pair of entries that have changed, where first is the old version and second is the new version.
     */
    void pathContentItemsChanged(QVector<QPair<FMH::MODEL, FMH::MODEL>> items);
    
    /**
     * @brief Emitted when a set of entries in the current location have been removed.
     * @param list the removed contents packaged in a list, with the file information 
     */
    void pathContentItemsRemoved(FMStatic::PATH_CONTENT list);

    /**
     * @brief Emitted when there is an error.
     * @param message the error message
     */
    void warningMessage(QString message);
    
    /**
     * @brief Emitted with the progress of the listing.
     * @param percent the progress percent form 0 to 100
     */
    void loadProgress(int percent);

    /**
     * @brief Emitted when a directory has been created in the remote server in the current location.
     * @param dir the information of the newly created directory
     */
    void dirCreated(FMH::MODEL dir);
    
    /**
     * @brief Emitted when a new item is available in the remote server in the current location.
     * @param item the new item information
     * @param path the path location of the new item
     */
    void newItem(FMH::MODEL item, QUrl path);

public Q_SLOTS:
    /**
     * @brief Open a given remote item in an external application. If the item does not exists in the system local cache, then  it is downloaded first.
     * @param item the item data. This is exposed this way for convenience of usage from QML, where the item entry from the model will become a QVariantMap.
     */
    void openCloudItem(const QVariantMap &item);
    
    /**
     * @brief Download a remote server entry.
     * @param item the item data.
     */
    void getCloudItem(const QVariantMap &item);

    //Actions
    /**
     * @brief Copy a set of file URLs to a new destination
     * @param urls the list of file URLs to be copied.
     * @param where the new location to copy the files
     */
    bool copy(const QList<QUrl> &urls, const QUrl &where);
    
    /**
     * @brief Cut a set of file URLs to a new destination
     * @param urls the list of file URLs to be cut.
     * @param where the new location to paste the files
     */
    bool cut(const QList<QUrl> &urls, const QUrl &where);

    friend class FMStatic;
};
