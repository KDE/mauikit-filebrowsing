#pragma once

#include <QList>
#include <QNetworkReply>
#include <QObject>
#include <QSslError>

#include "filebrowsing_export.h"
#include "../dto/WebDAVItem.hpp"

/**
 * @brief Wraps the available actions for a remote item
 */
class FILEBROWSING_EXPORT WebDAVReply : public QObject
{
    Q_OBJECT

public:
    void sendListDirResponseSignal(QNetworkReply* listDirReply,
                                   QList<WebDAVItem> items);
    void sendDownloadResponseSignal(QNetworkReply* downloadReply);
    void sendDownloadProgressResponseSignal(qint64 bytesReceived,
                                            qint64 bytesTotal);
    void sendUploadFinishedResponseSignal(QNetworkReply* uploadReply);
    void sendDirCreatedResponseSignal(QNetworkReply* createDirReply);
    void sendCopyResponseSignal(QNetworkReply* copyReply);
    void sendMoveResponseSignal(QNetworkReply* moveReply);
    void sendRemoveResponseSignal(QNetworkReply* removeReply);
    void sendError(QNetworkReply::NetworkError err);

Q_SIGNALS:
    void listDirResponse(QNetworkReply* listDirReply, QList<WebDAVItem> items);
    void downloadResponse(QNetworkReply* downloadReply);
    void downloadProgressResponse(qint64 bytesReceived, qint64 bytesTotal);
    void uploadFinished(QNetworkReply* uploadReply);
    void createDirFinished(QNetworkReply* createDirReply);
    void copyFinished(QNetworkReply* copyReply);
    void moveFinished(QNetworkReply* moveReply);
    void removeFinished(QNetworkReply* removeReply);
    void error(QNetworkReply::NetworkError err);
};
