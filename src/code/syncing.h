#pragma once

#include <QObject>
#include <QNetworkReply>

#include <QFile>

#include <MauiKit4/Core/fmh.h>

#include "filebrowsing_export.h"

class WebDAVClient;
class WebDAVReply;
/**
 * @brief The Syncing class
 */
class FILEBROWSING_EXPORT Syncing : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Syncing)
    
public:
    enum SIGNAL_TYPE : uint_fast8_t { OPEN, DOWNLOAD, COPY, SAVE, CUT, DELETE, RENAME, MOVE, UPLOAD };

    /**
     *  @brief uploadQueue
     */
    QStringList uploadQueue;

    /**
     * @brief Syncing
     * @param parent
     */
    explicit Syncing(QObject *parent = nullptr);

    /**
     * @brief listContent
     * @param path
     * @param filters
     * @param depth
     */
    void listContent(const QUrl &path, const QStringList &filters, const int &depth = 1);

    /**
     * @brief setCredentials
     * @param server
     * @param user
     * @param password
     */
    void setCredentials(const QString &server, const QString &user, const QString &password);

    /**
     * @brief download
     * @param path
     */
    void download(const QUrl &path);

    /**
     * @brief upload
     * @param path
     * @param filePath
     */
    void upload(const QUrl &path, const QUrl &filePath);

    /**
     * @brief createDir
     * @param path
     * @param name
     */
    void createDir(const QUrl &path, const QString &name);

    /**
     * @brief resolveFile
     * @param item
     * @param signalType
     */
    void resolveFile(const FMH::MODEL &item, const Syncing::SIGNAL_TYPE &signalType);

    /**
     * @brief setCopyTo
     * @param path
     */
    void setCopyTo(const QUrl &path);

    /**
     * @brief getCopyTo
     * @return
     */
    QUrl getCopyTo() const;

    /**
     * @brief getUser
     * @return
     */
    QString getUser() const;

    /**
     * @brief setUploadQueue
     * @param list
     */
    void setUploadQueue(const QStringList &list);

    /**
     * @brief localToAbstractCloudPath
     * @param url
     * @return
     */
    QString localToAbstractCloudPath(const QString &url);

private:
    WebDAVClient *client;
    QString host = QStringLiteral("https://cloud.opendesktop.cc/remote.php/webdav/");
    QString user = QStringLiteral("mauitest");
    QString password = QStringLiteral("mauitest");
    void listDirOutputHandler(WebDAVReply *reply, const QStringList &filters = QStringList());

    void saveTo(const QByteArray &array, const QUrl &path);
    QString saveToCache(const QString &file, const QUrl &where);
    QUrl getCacheFile(const QUrl &path);

    QUrl currentPath;
    QUrl copyTo;

    void emitError(const QNetworkReply::NetworkError &err);

    SIGNAL_TYPE signalType;

    QFile mFile;

Q_SIGNALS:
    /**
     * @brief listReady
     * @param data
     * @param url
     */
    void listReady(FMH::MODEL_LIST data, QUrl url);

    /**
     * @brief itemReady
     * @param item
     * @param url
     * @param signalType
     */
    void itemReady(FMH::MODEL item, QUrl url, Syncing::SIGNAL_TYPE &signalType);

    /**
     * @brief dirCreated
     * @param item
     * @param url
     */
    void dirCreated(FMH::MODEL item, QUrl url);

    /**
     * @brief uploadReady
     * @param item
     * @param url
     */
    void uploadReady(FMH::MODEL item, QUrl url);

    /**
     * @brief error
     * @param message
     */
    void error(QString message);

    /**
     * @brief progress
     * @param percent
     */
    void progress(int percent);
};
