#pragma once

#include <QByteArray>
#include <QList>

#include "../dto/WebDAVItem.hpp"

class WebDAVClient;

class XMLHelper {
 public:
  QList<WebDAVItem> parseListDirResponse(WebDAVClient *webdavClient,
                                         QByteArray xml);
};

