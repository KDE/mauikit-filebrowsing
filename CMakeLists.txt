# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.16)

option(BUILD_WITH_QT5 "Use Qt 5" OFF)
option(BUILD_WITH_QT6 "Use Qt 6" OFF)

if(BUILD_WITH_QT5)
    set(QT_MAJOR_VERSION 5)
elseif(BUILD_WITH_QT6)
    set(QT_MAJOR_VERSION 6)
else()
    set(QT_MAJOR_VERSION 5)
endif()

if (QT_MAJOR_VERSION STREQUAL "6")
    set(REQUIRED_QT_VERSION 6.4)
    set(REQUIRED_KF_VERSION 5.240.0)
    set(KF_MAJOR_VERSION 6)
    set(MAUI_MAJOR_VERSION 4)

    set(MAUIKIT_VERSION 4.0.0)

else()
    set(REQUIRED_QT_VERSION 5.15.2)
    set(REQUIRED_KF_VERSION 5.91.0)
    set(KF_MAJOR_VERSION 5)
    set(MAUI_MAJOR_VERSION 3)

    set(MAUIKIT_VERSION 3.0.1)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(AUTOMOC_MOC_OPTIONS -Muri=org.mauikit.filebrowsing)

project(FileBrowsing LANGUAGES CXX VERSION ${MAUIKIT_VERSION})

find_package(ECM ${REQUIRED_KF_VERSION} NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(GenerateExportHeader)
include(GNUInstallDirs)
include(FeatureSummary)
include(CMakePackageConfigHelpers)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(KDEPackageAppTemplates)
include(KDEClangFormat)

include(ECMSetupVersion)
include(ECMGenerateQmlTypes)
include(ECMGenerateHeaders)
include(ECMFindQmlModule)
include(ECMQmlModule)

if(ANDROID OR APPLE OR WIN32)
    set(IS_KIO_AVAILABLE OFF)
else()
    set(IS_KIO_AVAILABLE ON)
endif()

option (KIO_AVAILABLE "Use KIO library if it is avalibale, otherwise use default fallack implementation" ${IS_KIO_AVAILABLE})

option(QUICK_COMPILER "Use QtQuick compiler to improve performance" ON)

##option to be removed in qt6 migration
if (QT_MAJOR_VERSION STREQUAL "5")
    option(QUICK_COMPILER ON)
    if(ANDROID OR WIN32 OR APPLE)
        set(QUICK_COMPILER ON)
        set(BUNDLE_LUV_ICONS ON)
    endif()
else()
    option(QUICK_COMPILER OFF)
endif()

find_package(Qt${QT_MAJOR_VERSION} ${REQUIRED_QT_VERSION} REQUIRED COMPONENTS
    Core
    Quick
    Sql
    Xml
    Concurrent
    Network)

find_package(KF${KF_MAJOR_VERSION} ${REQUIRED_KF_VERSION} REQUIRED COMPONENTS
    I18n
    CoreAddons)

find_package(MauiKit${MAUI_MAJOR_VERSION} ${MAUIKIT_VERSION} REQUIRED)

if(ANDROID)
    find_package(Qt${QT_MAJOR_VERSION} REQUIRED COMPONENTS AndroidExtras)
    find_package(Gradle REQUIRED)
endif()

if(${KIO_AVAILABLE})
    find_package(KF${KF_MAJOR_VERSION} ${REQUIRED_KF_VERSION} REQUIRED COMPONENTS KIO)
endif()

if (QT_MAJOR_VERSION STREQUAL "5")
    if(QUICK_COMPILER)
        find_package(Qt5QuickCompiler)
        set_package_properties(Qt5QuickCompiler PROPERTIES
            DESCRIPTION "Compile QML at build time"
            TYPE OPTIONAL)
    endif()
endif()

##CMAKE PARTS
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/MauiKitFileBrowsing${MAUI_MAJOR_VERSION}")

ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX FileBrowsing
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/filebrowsing_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/MauiKitFileBrowsing${MAUI_MAJOR_VERSION}ConfigVersion.cmake"
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/MauiKitFileBrowsingConfig.cmake.in.${QT_MAJOR_VERSION}"
    "${CMAKE_CURRENT_BINARY_DIR}/MauiKitFileBrowsing${MAUI_MAJOR_VERSION}Config.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS CMAKE_INSTALL_PREFIX
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/MauiKitFileBrowsing${MAUI_MAJOR_VERSION}ConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/MauiKitFileBrowsing${MAUI_MAJOR_VERSION}Config.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT MauiKitFileBrowsing${MAUI_MAJOR_VERSION}Targets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE MauiKitFileBrowsing${MAUI_MAJOR_VERSION}Targets.cmake
    NAMESPACE MauiKit${MAUI_MAJOR_VERSION}::)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/filebrowsing_version.h"
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MauiKit${MAUI_MAJOR_VERSION}/FileBrowsing
    COMPONENT Devel)

add_definitions(-DTRANSLATION_DOMAIN="mauikitfilebrowsing")
ki18n_install(po)

add_subdirectory(src)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
