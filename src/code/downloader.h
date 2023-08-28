#pragma once

#include <QObject>
#include <QString>
#include <QMap>

#include "filebrowsing_export.h"

class QNetworkAccessManager;
class QNetworkReply;
class QFile;

namespace FMH
{
/**
 * @brief The Downloader class
 * This is a quick helper to download and save files. Allows to make get request using optional headers.
 */
class FILEBROWSING_EXPORT Downloader : public QObject
{
    Q_OBJECT
   
public:
    Downloader(QObject *parent = nullptr);

    virtual ~Downloader();

    /**
     * @brief downloadFile
     * Given a source URL to a file it downloads it to a given local destination
     * @param source
     * Remote file URL
     * @param destination
     * Local file URL destination
     */
    void downloadFile(const QUrl &source, const QUrl &destination);

    /**
     * @brief getArray
     * Given a URL make a get request and once the reply is ready emits a signal with the retrieved array data
     * @param fileURL
     * @param headers
     */
    void getArray(const QUrl &fileURL, const QMap<QString, QString> &headers = {});
    
    void stop();
    bool isRunning() const;
    bool isFinished() const;

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile *file;
    QByteArray *array;
    
    bool m_saveToFile = false;
    void setConnections();
    
Q_SIGNALS:
    void progress(int percent);
    void downloadReady();
    void aborted();
    void fileSaved(QString path);
    void warning(QString warning);
    void dataReady(QByteArray array);
    void done();

private Q_SLOTS:
    /**
     * @brief onDownloadProgress
     * @param bytesRead
     * @param bytesTotal
     */
    void onDownloadProgress(qint64 bytesRead, qint64 bytesTotal);

    /**
     * @brief onReadyRead
     */
    void onReadyRead();

    /**
     * @brief onReplyFinished
     */
    void onReplyFinished();
};
}
