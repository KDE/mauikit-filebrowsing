#pragma once

#include <QIODevice>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>
#include <QString>

#include "filebrowsing_export.h"

#include "utils/NetworkHelper.hpp"
#include "utils/WebDAVReply.hpp"
#include "utils/XMLHelper.hpp"

enum ListDepthEnum { Zero, One, Two, Infinity };

class FILEBROWSING_EXPORT WebDAVClient : public QObject
{
    Q_OBJECT

public:
    WebDAVClient(QString host, QString username, QString password);

    WebDAVReply* listDir(QString path = QStringLiteral("/"));
    WebDAVReply* listDir(QString path, ListDepthEnum depth);

    WebDAVReply* downloadFrom(QString path);
    WebDAVReply* downloadFrom(QString path, qint64 startByte, qint64 endByte);

    WebDAVReply* uploadTo(QString path, QString filename, QIODevice* file);

    WebDAVReply* createDir(QString path, QString dirName);

    WebDAVReply* copy(QString source, QString destination);

    WebDAVReply* move(QString source, QString destination,
                      bool overwrite = false);

    WebDAVReply* remove(QString path);

    ~WebDAVClient();

private:
    NetworkHelper* networkHelper;
    XMLHelper* xmlHelper;

    void errorReplyHandler(WebDAVReply* reply, QNetworkReply::NetworkError err);
};
