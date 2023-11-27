#include "moduleinfo.h"
#include "filebrowsing_version.h"
#include <KI18n/KLocalizedString>

QString MauiKitFileBrowsing::versionString()
{
    return QStringLiteral(FileBrowsing_VERSION_STRING);
}

QString MauiKitFileBrowsing::buildVersion()
{
    return QStringLiteral(GIT_BRANCH)+QStringLiteral("/")+QStringLiteral(GIT_COMMIT_HASH);
}

KAboutComponent MauiKitFileBrowsing::aboutData()
{
    return KAboutComponent(QStringLiteral("MauiKit FileBrowsing"),
                         i18n("File browsing and management components."),
                         QStringLiteral(FileBrowsing_VERSION_STRING),
                         QStringLiteral("http://mauikit.org"),
                         KAboutLicense::LicenseKey::LGPL_V3);
}
