// SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "filebrowsing_plugin.h"

#include <QQmlEngine>
#include <QResource>
#include <QDebug>

#include "thumbnailer.h"

#include "fmstatic.h"

#include "tagslist.h"
#include "tagging.h"

#include "placeslist.h"
#include "fmlist.h"
#include "openwithmodel.h"

QUrl FileBrowsingPlugin::componentUrl(const QString &fileName) const
{
    return QUrl(resolveFileUrl(fileName));
}

void FileBrowsingPlugin::registerTypes(const char *uri)
{
    qDebug() << "REGISTER MAUIKITFILEBROWSING TYPES <<<<<<<<<<<<<<<<<<<<<<";

#if defined(Q_OS_ANDROID)
    QResource::registerResource(QStringLiteral("assets:/android_rcc_bundle.rcc"));
#endif

    Q_ASSERT(QLatin1String(uri) == QLatin1String("org.mauikit.filebrowsing"));
    
    //File Browsing components
    qmlRegisterType<FMList>(uri, 1, 0, "FMList");
    qmlRegisterType<PlacesList>(uri, 1, 0, "PlacesList");
    qmlRegisterUncreatableType<PathStatus>(uri, 1, 0, "PathStatus", QStringLiteral("cannot be created :: PathStatus"));

    qmlRegisterType(componentUrl(QStringLiteral("FileBrowser.qml")), uri, 1, 0, "FileBrowser");
    qmlRegisterType(componentUrl(QStringLiteral("PlacesListBrowser.qml")), uri, 1, 0, "PlacesListBrowser");
    qmlRegisterType(componentUrl(QStringLiteral("FileDialog.qml")), uri, 1, 0, "FileDialog");

    qmlRegisterType<OpenWithModel>(uri, 1, 3, "OpenWithModel");
    qmlRegisterType(componentUrl(QStringLiteral("OpenWithDialog.qml")), uri, 1, 0, "OpenWithDialog");
    
    qmlRegisterSingletonType<FMStatic>(uri, 1, 0, "FM", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)
        return new FMStatic;
    });
    
    //File Tagging components
    qmlRegisterSingletonType<Tagging>(uri, 1, 3, "Tagging", &Tagging::qmlInstance);
    
    qmlRegisterType<TagsList>(uri, 1, 0, "TagsListModel");
    qmlRegisterType(componentUrl(QStringLiteral("private/TagList.qml")), uri, 1, 0, "TagList");
    qmlRegisterType(componentUrl(QStringLiteral("TagsBar.qml")), uri, 1, 0, "TagsBar");
    qmlRegisterType(componentUrl(QStringLiteral("TagsDialog.qml")), uri, 1, 0, "TagsDialog");
    qmlRegisterType(componentUrl(QStringLiteral("NewTagDialog.qml")), uri, 1, 3, "NewTagDialog");
    
    //File Syncing components
}

void FileBrowsingPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    /** IMAGE PROVIDERS **/
    engine->addImageProvider(QStringLiteral("thumbnailer"), new Thumbnailer());
}
