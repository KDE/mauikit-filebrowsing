# SPDX-FileCopyrightText: 2020 Camilo Higuita <milo.h@aol.com>
#
# SPDX-License-Identifier: GPL-3.0-or-later

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/code
    ${CMAKE_CURRENT_SOURCE_DIR}/code/libwebdavclient/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/code/libwebdavclient/lib/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/code/libwebdavclient/lib/dto)

add_library(MauiKitFileBrowsing${MAUI_MAJOR_VERSION} SHARED)

ecm_add_qml_module(MauiKitFileBrowsing${MAUI_MAJOR_VERSION}
    VERSION 1.3
    URI "org.mauikit.filebrowsing"
    CLASS_NAME FileBrowsingPlugin
    NO_PLUGIN_OPTIONAL
    NO_GENERATE_PLUGIN_SOURCE
    DEPENDENCIES
    "org.mauikit.controls")

set_target_properties(MauiKitFileBrowsing${MAUI_MAJOR_VERSION} PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    EXPORT_NAME "FileBrowsing")

add_library(MauiKit${MAUI_MAJOR_VERSION}::FileBrowsing ALIAS MauiKitFileBrowsing${MAUI_MAJOR_VERSION})

set(headers
    code/fm.h
    code/fmstatic.h
    code/fmlist.h
    code/placeslist.h
    code/downloader.h
    code/fileloader.h
    code/tagging.h
    code/tagdb.h
    code/moduleinfo.h)

set(syncing_SRCS
    code/syncing.cpp
    #code/syncinglist.cpp
    code/libwebdavclient/lib/WebDAVClient.cpp
    code/libwebdavclient/lib/dto/WebDAVItem.cpp
    code/libwebdavclient/lib/utils/Environment.cpp
    code/libwebdavclient/lib/utils/NetworkHelper.cpp
    code/libwebdavclient/lib/utils/WebDAVReply.cpp
    code/libwebdavclient/lib/utils/XMLHelper.cpp)

set(syncing_HDRS
    code/syncing.h
    #code/syncinglist.h
    code/libwebdavclient/lib/WebDAVClient.hpp
    code/libwebdavclient/lib/dto/WebDAVItem.hpp
    code/libwebdavclient/lib/utils/Environment.hpp
    code/libwebdavclient/lib/utils/NetworkHelper.hpp
    code/libwebdavclient/lib/utils/WebDAVReply.hpp
    code/libwebdavclient/lib/utils/XMLHelper.hpp)

qt_add_resources(filebrowsing_QML_QRC assets.qrc) #Maybe it is needed by Android?

ecm_target_qml_sources(MauiKitFileBrowsing${MAUI_MAJOR_VERSION} SOURCES

    controls.${QT_MAJOR_VERSION}/FileBrowser.qml
    controls.${QT_MAJOR_VERSION}/OpenWithDialog.qml
    controls.${QT_MAJOR_VERSION}/FileDialog.qml
    controls.${QT_MAJOR_VERSION}/NewTagDialog.qml
    controls.${QT_MAJOR_VERSION}/PlacesListBrowser.qml
    controls.${QT_MAJOR_VERSION}/TagsDialog.qml
    controls.${QT_MAJOR_VERSION}/TagsBar.qml)

ecm_target_qml_sources(MauiKitFileBrowsing${MAUI_MAJOR_VERSION} PATH private SOURCES

    controls.${QT_MAJOR_VERSION}/private/BrowserHolder.qml
    controls.${QT_MAJOR_VERSION}/private/BrowserSettings.qml

    controls.${QT_MAJOR_VERSION}/private/BrowserView.qml

    controls.${QT_MAJOR_VERSION}/private/TagDelegate.qml
    controls.${QT_MAJOR_VERSION}/private/TagList.qml)

target_sources(MauiKitFileBrowsing${MAUI_MAJOR_VERSION}
    PRIVATE
    code/fm.cpp
    code/fm.h

    code/fmstatic.cpp
    code/fmstatic.h

    code/fmlist.cpp
    code/fmlist.h

    code/placeslist.cpp
    code/placeslist.h

    code/downloader.cpp
    code/downloader.h

    code/fileloader.cpp
    code/fileloader.h

    code/tagging.cpp
    code/tagging.h

    code/tagdb.cpp
    code/tagdb.h

    code/tagging.qrc

    code/moduleinfo.cpp
    code/moduleinfo.h
    ${syncing_SRCS})

target_sources(MauiKitFileBrowsing${MAUI_MAJOR_VERSION}plugin
    PRIVATE
    code/filebrowsing_plugin.cpp
    code/filebrowsing_plugin.h

    code/thumbnailer.cpp
    code/thumbnailer.h

    code/tagslist.cpp
    code/tagslist.h

    code/openwithmodel.cpp
    code/openwithmodel.h

    ${filebrowsing_QML_QRC})

generate_export_header(MauiKitFileBrowsing${MAUI_MAJOR_VERSION} BASE_NAME FileBrowsing)

target_include_directories(MauiKitFileBrowsing${MAUI_MAJOR_VERSION}
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/MauiKit${MAUI_MAJOR_VERSION}/FileBrowsing>")

target_include_directories(MauiKitFileBrowsing${MAUI_MAJOR_VERSION} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

if(${KIO_AVAILABLE})
    target_link_libraries(MauiKitFileBrowsing${MAUI_MAJOR_VERSION}
        PRIVATE
        KF${KF_MAJOR_VERSION}::KIOCore
        KF${KF_MAJOR_VERSION}::KIOFileWidgets)

    target_link_libraries(MauiKitFileBrowsing${MAUI_MAJOR_VERSION}plugin
        PRIVATE
        KF${KF_MAJOR_VERSION}::KIOCore
        KF${KF_MAJOR_VERSION}::KIOFileWidgets)

    add_definitions(-DKIO_AVAILABLE)
    target_compile_definitions(MauiKitFileBrowsing${MAUI_MAJOR_VERSION} PUBLIC KIO_AVAILABLE)
endif()

target_link_libraries(MauiKitFileBrowsing${MAUI_MAJOR_VERSION}
    PRIVATE
    KF${KF_MAJOR_VERSION}::I18n
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Quick
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Sql
    Qt${QT_MAJOR_VERSION}::Xml
    Qt${QT_MAJOR_VERSION}::Concurrent
    Qt${QT_MAJOR_VERSION}::Network
    MauiKit${MAUI_MAJOR_VERSION})

target_link_libraries(MauiKitFileBrowsing${MAUI_MAJOR_VERSION}plugin
    PRIVATE
    MauiKitFileBrowsing${MAUI_MAJOR_VERSION}
    MauiKit${MAUI_MAJOR_VERSION}
    KF${KF_MAJOR_VERSION}::I18n
    Qt${QT_MAJOR_VERSION}::Core
    Qt${QT_MAJOR_VERSION}::Quick
    Qt${QT_MAJOR_VERSION}::Qml
    Qt${QT_MAJOR_VERSION}::Sql
    Qt${QT_MAJOR_VERSION}::Xml
    Qt${QT_MAJOR_VERSION}::Concurrent
    Qt${QT_MAJOR_VERSION}::Network)

ecm_finalize_qml_module(MauiKitFileBrowsing${MAUI_MAJOR_VERSION} DESTINATION ${KDE_INSTALL_QMLDIR})

install(TARGETS MauiKitFileBrowsing${MAUI_MAJOR_VERSION} EXPORT MauiKitFileBrowsing${MAUI_MAJOR_VERSION}Targets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${headers}
    ${syncing_HDRS}
    ${CMAKE_CURRENT_BINARY_DIR}/filebrowsing_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MauiKit${MAUI_MAJOR_VERSION}/FileBrowsing
    COMPONENT Devel)


