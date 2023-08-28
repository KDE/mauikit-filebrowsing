#include <QByteArray>
#include <QDebug>
#include <QList>
#include <QtXml/QDomDocument>

#include "../dto/WebDAVItem.hpp"
#include "XMLHelper.hpp"

QList<WebDAVItem> XMLHelper::parseListDirResponse(WebDAVClient *webdavClient, QByteArray xml)
{
    QList<WebDAVItem> items;
    QString webdavNS = QStringLiteral("DAV:");
    QDomDocument doc;
    doc.setContent(xml, true);

    QDomNodeList responses = doc.elementsByTagNameNS(webdavNS, QStringLiteral("response"));

    for (int i = 0; i < responses.length(); i++) {
        QDomElement response = responses.at(i).toElement();

        QString href = response.elementsByTagNameNS(webdavNS, QStringLiteral("href")).at(0).toElement().text();
        QString creationDate = response.elementsByTagNameNS(webdavNS, QStringLiteral("creationdate")).at(0).toElement().text();
        QString lastModified = response.elementsByTagNameNS(webdavNS, QStringLiteral("getlastmodified")).at(0).toElement().text();
        QString displayName = response.elementsByTagNameNS(webdavNS, QStringLiteral("displayname")).at(0).toElement().text();
        QString contentType = response.elementsByTagNameNS(webdavNS, QStringLiteral("getcontenttype")).at(0).toElement().text();
        QString contentLength = response.elementsByTagNameNS(webdavNS, QStringLiteral("getcontentlength")).at(0).toElement().text();
        bool isCollection;

        if (response.elementsByTagNameNS(webdavNS, QStringLiteral("resourcetype")).at(0).toElement().elementsByTagNameNS(webdavNS, QStringLiteral("collection")).size() == 1) {
            isCollection = true;
        } else {
            isCollection = false;
        }

        items.append(WebDAVItem(webdavClient, href, creationDate, lastModified, displayName, contentType, contentLength, isCollection));
    }

    return items;
}
