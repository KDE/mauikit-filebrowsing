// SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QQmlEngine>
#include <QResource>

#include "filebrowsing_plugin.h"
#include "placeslist.h"
#include "fmlist.h"

void FileBrowsingPlugin::registerTypes(const char *uri)
{
#if defined(Q_OS_ANDROID)
    QResource::registerResource(QStringLiteral("assets:/android_rcc_bundle.rcc"));
#endif

    qmlRegisterType<PlacesList>(uri, 1, 0, "PlacesList");
    qmlRegisterType<FMList>(uri, 1, 0, "FMList");
    qmlRegisterType(resolveFileUrl(QStringLiteral("FileBrowser.qml")), uri, 1, 0, "FileBrowser");
    qmlRegisterType(resolveFileUrl(QStringLiteral("PlacesListBrowser.qml")), uri, 1, 0, "PlacesListBrowser");
    qmlRegisterType(resolveFileUrl(QStringLiteral("FileDialog.qml")), uri, 1, 0, "FileDialog");
}
