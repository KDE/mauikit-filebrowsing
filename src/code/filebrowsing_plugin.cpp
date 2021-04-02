// SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QQmlEngine>
#include <QResource>

#include "filebrowsing_plugin.h"
#include "placeslist.h"
#include "fmlist.h"
#include "thumbnailer.h"
#include "fmstatic.h"

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
    
        qmlRegisterSingletonType<FMStatic>(uri, 1, 0, "FM", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return new FMStatic;
        });
}

void FileBrowsingPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
  
    /** IMAGE PROVIDERS **/
    engine->addImageProvider("thumbnailer", new Thumbnailer());
}
