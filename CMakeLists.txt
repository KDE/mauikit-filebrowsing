# SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
#
# SPDX-License-Identifier: BSD-2-Clause

cmake_minimum_required(VERSION 3.5)

set(KF5_VERSION "5.70.0")
set(REQUIRED_QT_VERSION 5.15.0)

set(CMAKE_CXX_STANDARD 17)

project(FileBrowsing LANGUAGES CXX VERSION 1.2.2)

find_package(ECM ${KF5_VERSION} NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(GenerateExportHeader)
include(GNUInstallDirs)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)
include(ECMPoQmTools)
include(ECMSetupVersion)
include(CMakePackageConfigHelpers)
#include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDEPackageAppTemplates)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/MauiKitFileBrowsing")

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/MauiKitFileBrowsingConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/MauiKitFileBrowsingConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
    PATH_VARS KF5_INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/MauiKitFileBrowsingConfigVersion.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/MauiKitFileBrowsingConfig.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

    install(EXPORT MauiKitFileBrowsingTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE MauiKitFileBrowsingTargets.cmake
        NAMESPACE MauiKit::)
        
ecm_setup_version(${PROJECT_VERSION}
    VARIABLE_PREFIX FileBrowsing
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/filebrowsing_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/MauiKitFileBrowsingConfigVersion.cmake"
    SOVERSION 1
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/filebrowsing_version.h"
        DESTINATION "${KDE_INSTALL_INCLUDEDIR_KF5}"
        COMPONENT Devel)
                
find_package(Qt5 ${REQUIRED_QT_VERSION} COMPONENTS Core Quick Sql Concurrent Network REQUIRED)
find_package(KF5 ${REQUIRED_KF5_VERSION} REQUIRED COMPONENTS I18n Config KIO)
find_package(MauiKit REQUIRED)
find_package(MauiKit OPTIONAL_COMPONENTS FileTagging)
    
add_subdirectory(src)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
