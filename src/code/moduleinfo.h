#pragma once

#include <QString>
#include <KAboutData>
#include "filebrowsing_export.h"

namespace MauiKitFileBrowsing
{
   FILEBROWSING_EXPORT QString versionString();
   FILEBROWSING_EXPORT QString buildVersion();
   FILEBROWSING_EXPORT KAboutComponent aboutData();
};
