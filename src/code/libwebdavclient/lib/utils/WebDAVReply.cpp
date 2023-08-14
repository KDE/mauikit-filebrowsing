#include <QByteArray>
#include <QList>
#include <QNetworkReply>
#include <QSslError>

#include "WebDAVReply.hpp"

void WebDAVReply::sendListDirResponseSignal(QNetworkReply *listDirReply, QList<WebDAVItem> items)
{
    Q_EMIT listDirResponse(listDirReply, items);
}

void WebDAVReply::sendDownloadResponseSignal(QNetworkReply *downloadReply)
{
    Q_EMIT downloadResponse(downloadReply);
}

void WebDAVReply::sendDownloadProgressResponseSignal(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_EMIT downloadProgressResponse(bytesReceived, bytesTotal);
}

void WebDAVReply::sendUploadFinishedResponseSignal(QNetworkReply *uploadReply)
{
    Q_EMIT uploadFinished(uploadReply);
}

void WebDAVReply::sendDirCreatedResponseSignal(QNetworkReply *createDirReply)
{
    Q_EMIT createDirFinished(createDirReply);
}

void WebDAVReply::sendCopyResponseSignal(QNetworkReply *copyReply)
{
    Q_EMIT copyFinished(copyReply);
}

void WebDAVReply::sendMoveResponseSignal(QNetworkReply *moveReply)
{
    Q_EMIT moveFinished(moveReply);
}

void WebDAVReply::sendRemoveResponseSignal(QNetworkReply *removeReply)
{
    Q_EMIT removeFinished(removeReply);
}

void WebDAVReply::sendError(QNetworkReply::NetworkError err)
{
    Q_EMIT error(err);
}
