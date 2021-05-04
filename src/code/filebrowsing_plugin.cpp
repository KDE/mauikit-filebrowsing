// SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include <QQmlEngine>
#include <QResource>

#include "filebrowsing_plugin.h"

#include "thumbnailer.h"

#include "fmstatic.h"

#include "tagslist.h"
#include "tagging.h"

#include "placeslist.h"
#include "fmlist.h"
#include "openwithmodel.h"

void FileBrowsingPlugin::registerTypes(const char *uri)
{
#if defined(Q_OS_ANDROID)
    QResource::registerResource(QStringLiteral("assets:/android_rcc_bundle.rcc"));
    #endif
    
    //File Browsing components
    qmlRegisterType<PlacesList>(uri, 1, 0, "PlacesList");
    qmlRegisterUncreatableType<PathStatus>(uri, 1, 0, "PathStatus", "cannot be created :: PathStatus"); 
    qmlRegisterType<FMList>(uri, 1, 0, "FMList");
    qmlRegisterType(resolveFileUrl(QStringLiteral("FileBrowser.qml")), uri, 1, 0, "FileBrowser");
    qmlRegisterType(resolveFileUrl(QStringLiteral("PlacesListBrowser.qml")), uri, 1, 0, "PlacesListBrowser");
    qmlRegisterType(resolveFileUrl(QStringLiteral("FileDialog.qml")), uri, 1, 0, "FileDialog");

    qmlRegisterType<OpenWithModel>(uri, 1, 3, "OpenWithModel");
    qmlRegisterType(resolveFileUrl(QStringLiteral("OpenWithDialog.qml")), uri, 1, 0, "OpenWithDialog");
    
    qmlRegisterSingletonType<FMStatic>(uri, 1, 0, "FM", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new FMStatic;
    });    
    
    //File Tagging components
    qmlRegisterSingletonType<Tagging>(uri, 1, 3, "Tagging", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return Tagging::getInstance();
    }); //the singleton instance results in having tagging instance created in different threads which is not supported byt the slq driver
    
    qmlRegisterType<TagsList>(uri, 1, 0, "TagsListModel");
    qmlRegisterType(resolveFileUrl(QStringLiteral("private/TagList.qml")), uri, 1, 0, "TagList");
    qmlRegisterType(resolveFileUrl(QStringLiteral("TagsBar.qml")), uri, 1, 0, "TagsBar");
    qmlRegisterType(resolveFileUrl(QStringLiteral("TagsDialog.qml")), uri, 1, 0, "TagsDialog");
    qmlRegisterType(resolveFileUrl(QStringLiteral("NewTagDialog.qml")), uri, 1, 3, "NewTagDialog");
    
    //File Syncing components
}

void FileBrowsingPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri);
  
    /** IMAGE PROVIDERS **/
    engine->addImageProvider("thumbnailer", new Thumbnailer());
}
