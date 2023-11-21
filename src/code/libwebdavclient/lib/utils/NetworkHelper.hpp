#pragma once

#include <QIODevice>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>

/**
 * @private
 */
class NetworkHelper : public QObject {
  Q_OBJECT

 private:
  QString host;
  QString username;
  QString password;
  QNetworkAccessManager* networkManager;

  void setRequestAuthHeader(QNetworkRequest* request);
  void setRequestHeaders(QNetworkRequest* request,
                         QMap<QString, QString> headers);

 public:
  NetworkHelper(QString host, QString username, QString password);

  QNetworkReply* makeRequest(QString method, QString path,
                             QMap<QString, QString> headers);
  QNetworkReply* makePutRequest(QString path, QMap<QString, QString> headers,
                                QIODevice* file);
};
