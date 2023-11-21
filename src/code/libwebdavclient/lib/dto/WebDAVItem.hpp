#pragma once

#include <QDateTime>
#include <QIODevice>
#include <QString>

class WebDAVClient;
class WebDAVReply;

/**
 * @brief Represents an item in a remote cloud location
 */
class WebDAVItem
{
 public:
   /**
    * @brief
    */
  WebDAVItem(WebDAVClient* webdavClient, QString href, QString creationDate,
             QString lastModified, QString displayName, QString contentType,
             QString contentLength, bool isCollection);

  /**
    * @brief
    */
  bool isCollection();
  
  /**
    * @brief
    */
  bool isFile();

  /**
    * @brief
    */
  WebDAVReply* download();
  
  /**
    * @brief
    */
  WebDAVReply* listDir();
  
  /**
    * @brief
    */
  WebDAVReply* upload(QString filename, QIODevice* file);
  
  /**
    * @brief
    */
  WebDAVReply* createDir(QString dirName);
  
  /**
    * @brief
    */
  WebDAVReply* copy(QString destination);
  
  /**
    * @brief
    */
  WebDAVReply* move(QString destination, bool overwrite = false);
  
  /**
    * @brief
    */
  WebDAVReply* remove();

  /**
    * @brief
    */
  QString toString();

  /**
    * @brief
    */
  QString getHref();
  
  /**
    * @brief
    */
  QDateTime getCreationDate();
  
  /**
    * @brief
    */
  QString getLastModified();
  
  /**
    * @brief
    */
  QString getDisplayName();
  
  /**
    * @brief
    */
  QString getContentType();
  
  /**
    * @brief
    */
  int getContentLength();

 private:
  WebDAVClient* webdavClient;
  QString href;
  QDateTime creationDate;
  QString lastModified;
  QString displayName;
  QString contentType;
  int contentLength;

  bool flagIsCollection;
};

