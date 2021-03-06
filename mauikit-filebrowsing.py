import info
from Package.CMakePackageBase import *


class subinfo(info.infoclass):
    def setTargets(self):
        self.svnTargets['master'] = 'https://invent.kde.org/maui/mauikit-filebrowsing.git'

        for ver in ['1.2.2']:
            self.targets[ver] = 'https://download.kde.org/stable/maui/mauikit-filebrowsing/1.2.2/mauikit-filebrowsing-%s.tar.xz' % ver
            self.archiveNames[ver] = 'mauikit-filebrowsing-%s.tar.gz' % ver
            self.targetInstSrc[ver] = 'mauikit-filebrowsing-%s' % ver

        self.description = "MauiKit File Browsing utilities and controls"
        self.defaultTarget = '1.2.2'

    def setDependencies(self):
        self.runtimeDependencies["virtual/base"] = None
        self.runtimeDependencies["libs/qt5/qtbase"] = None


class Package(CMakePackageBase):
    def __init__(self, **args):
        CMakePackageBase.__init__(self)
