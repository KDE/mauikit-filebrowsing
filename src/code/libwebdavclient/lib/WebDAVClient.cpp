#include <QAuthenticator>
#include <QByteArray>
#include <QDebug>
#include <QHttpMultiPart>
#include <QList>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QRegularExpression>
#include <QSslError>
#include <string>

#include "WebDAVClient.hpp"
#include "utils/NetworkHelper.hpp"
#include "utils/WebDAVReply.hpp"

WebDAVClient::WebDAVClient(QString host, QString username, QString password)
{
    this->networkHelper = new NetworkHelper(host, username, password);
    this->xmlHelper = new XMLHelper();

    // TODO: Check for Timeout error in case of wrong host
}

WebDAVReply *WebDAVClient::listDir(QString path)
{
    return this->listDir(path, ListDepthEnum::Infinity);
}

WebDAVReply *WebDAVClient::listDir(QString path, ListDepthEnum depth)
{
    WebDAVReply *reply = new WebDAVReply();
    QString depthVal;
    QMap<QString, QString> headers;
    QNetworkReply *listDirReply;

    switch (depth) {
    case ListDepthEnum::Zero:
        depthVal = QStringLiteral("0");
        break;

    case ListDepthEnum::One:
        depthVal = QStringLiteral("1");
        break;

    case ListDepthEnum::Two:
        depthVal = QStringLiteral("2");
        break;

    case ListDepthEnum::Infinity:
        depthVal = QStringLiteral("infinity");
        break;

    default:
        break;
    }

    headers.insert(QStringLiteral("Depth"), depthVal);

    listDirReply = this->networkHelper->makeRequest(QStringLiteral("PROPFIND"), path, headers);

    connect(listDirReply, &QNetworkReply::finished, [=]() {
        reply->sendListDirResponseSignal(listDirReply, this->xmlHelper->parseListDirResponse(this, listDirReply->readAll()));
    });
    connect(listDirReply, &QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError err) {
        this->errorReplyHandler(reply, err);
    });

    return reply;
}

WebDAVReply *WebDAVClient::downloadFrom(QString path)
{
    return this->downloadFrom(path, 0, -1);
}

WebDAVReply *WebDAVClient::downloadFrom(QString path, qint64 startByte, qint64 endByte)
{
    WebDAVReply *reply = new WebDAVReply();
    QString rangeVal;
    QTextStream stream(&rangeVal);
    QMap<QString, QString> headers;
    QNetworkReply *downloadReply;

    stream << "bytes=" << startByte << "-" << endByte;

    headers.insert(QStringLiteral("Range"), rangeVal);

    downloadReply = this->networkHelper->makeRequest(QStringLiteral("GET"), path, headers);

    connect(downloadReply, &QNetworkReply::finished, [=]() {
        reply->sendDownloadResponseSignal(downloadReply);
    });

    connect(downloadReply, &QNetworkReply::downloadProgress, [=](qint64 bytesReceived, qint64 bytesTotal) {
        if (bytesTotal == -1) {
            QString contentRange = QString::fromStdString(downloadReply->rawHeader("Content-Range").toStdString());
            QRegularExpression re(QStringLiteral("bytes (\\d*)-(\\d*)/(\\d*)"));
            QRegularExpressionMatch match = re.match(contentRange);
            qint64 contentSize = match.captured(2).toInt() - match.captured(1).toInt();

            reply->sendDownloadProgressResponseSignal(bytesReceived, contentSize);
        } else {
            reply->sendDownloadProgressResponseSignal(bytesReceived, bytesTotal);
        }
    });
    connect(downloadReply, &QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError err) {
        this->errorReplyHandler(reply, err);
    });

    return reply;
}

WebDAVReply *WebDAVClient::uploadTo(QString path, QString filename, QIODevice *file)
{
    WebDAVReply *reply = new WebDAVReply();
    QMap<QString, QString> headers;
    QNetworkReply *uploadReply;

    uploadReply = this->networkHelper->makePutRequest(path + QStringLiteral("/") + filename, headers, file);

    connect(uploadReply, &QNetworkReply::finished, [=]() {
        reply->sendUploadFinishedResponseSignal(uploadReply);
    });

    connect(uploadReply, &QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError err) {
        this->errorReplyHandler(reply, err);
    });

    return reply;
}

WebDAVReply *WebDAVClient::createDir(QString path, QString dirName)
{
    WebDAVReply *reply = new WebDAVReply();
    QMap<QString, QString> headers;
    QNetworkReply *createDirReply;

    createDirReply = this->networkHelper->makeRequest(QStringLiteral("MKCOL"), path + QStringLiteral("/") + dirName, headers);

    connect(createDirReply, &QNetworkReply::finished, [=]() {
        reply->sendDirCreatedResponseSignal(createDirReply);
    });

    connect(createDirReply,&QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError err) {
        this->errorReplyHandler(reply, err);
    });

    return reply;
}

WebDAVReply *WebDAVClient::copy(QString source, QString destination)
{
    WebDAVReply *reply = new WebDAVReply();
    QMap<QString, QString> headers;
    QNetworkReply *copyReply;

    headers.insert(QStringLiteral("Destination"), destination);

    copyReply = this->networkHelper->makeRequest(QStringLiteral("COPY"), source, headers);

    connect(copyReply, &QNetworkReply::finished, [=]() {
        reply->sendCopyResponseSignal(copyReply);
    });

    connect(copyReply, &QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError err) {
        this->errorReplyHandler(reply, err);
    });

    return reply;
}

WebDAVReply *WebDAVClient::move(QString source, QString destination, bool overwrite)
{
    WebDAVReply *reply = new WebDAVReply();
    QMap<QString, QString> headers;
    QNetworkReply *moveReply;
    QString overwriteVal = overwrite ? QStringLiteral("T") : QStringLiteral("F");

    headers.insert(QStringLiteral("Destination"), destination);
    headers.insert(QStringLiteral("Overwrite"), overwriteVal);

    moveReply = this->networkHelper->makeRequest(QStringLiteral("MOVE"), source, headers);

    connect(moveReply, &QNetworkReply::finished, [=]() {
        reply->sendMoveResponseSignal(moveReply);
    });

    connect(moveReply,&QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError err) {
        this->errorReplyHandler(reply, err);
    });

    return reply;
}

WebDAVReply *WebDAVClient::remove(QString path)
{
    WebDAVReply *reply = new WebDAVReply();
    QMap<QString, QString> headers;
    QNetworkReply *removeReply;

    removeReply = this->networkHelper->makeRequest(QStringLiteral("DELETE"), path, headers);

    connect(removeReply, &QNetworkReply::finished, [=]() {
        reply->sendRemoveResponseSignal(removeReply);
    });

    connect(removeReply, &QNetworkReply::errorOccurred, [=](QNetworkReply::NetworkError err) {
        this->errorReplyHandler(reply, err);
    });

    return reply;
}

void WebDAVClient::errorReplyHandler(WebDAVReply *reply, QNetworkReply::NetworkError err)
{
    reply->sendError(err);
}

WebDAVClient::~WebDAVClient()
{
    this->networkHelper->deleteLater();
    delete this->xmlHelper;
}
