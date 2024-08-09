// SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
//
// SPDX-License-Identifier: LGPL-2.1-or-later

#pragma once

#include <QDir>
#include <QQmlExtensionPlugin>

/**
 * @private
 */
class FileBrowsingPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)
    
public:
    void registerTypes(const char *uri) override;
    
private:
    QUrl componentUrl(const QString &fileName) const;
    
    void initializeEngine(QQmlEngine *engine, const char *uri) override;
    
    QString resolveFileUrl(const QString &filePath) const
    {
#if defined(Q_OS_ANDROID)
        return QStringLiteral(":/qt/qml/org/mauikit/filebrowsing/") + filePath;
#else
        return baseUrl().toString() + QLatin1Char('/') + filePath;
#endif
    }
};
