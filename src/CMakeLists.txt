# SPDX-FileCopyrightText: 2020 Carl Schwan <carl@carlschwan.eu>
#
# SPDX-License-Identifier: BSD-2-Clause

ecm_create_qm_loader(filebrowsing_QM_LOADER libfilebrowsingplugin_qt)

set(
    sources
    code/filebrowsing_plugin.cpp
    code/fm.cpp
    code/fmlist.cpp
    code/placeslist.cpp
    code/downloader.cpp
    code/fileloader.cpp
    code/thumbnailer.cpp
    resources.qrc
    ${filebrowsing_QM_LOADER}
)

set(
    headers
    code/fm.h
    code/fmlist.h
    code/placeslist.h
    code/downloader.h
    code/fileloader.h
    code/thumbnailer.h
)

set(
    pluginData
    controls/qmldir
)

add_library(
    MauiKitFileBrowsing
    SHARED
    ${sources} ${pluginData}
)

add_library(MauiKit::FileBrowsing ALIAS MauiKitFileBrowsing)

generate_export_header(MauiKitFileBrowsing BASE_NAME FileBrowsing)
set_target_properties(MauiKitFileBrowsing PROPERTIES
    VERSION ${FileBrowsing_VERSION_STRING}
    SOVERSION ${FileBrowsing_SOVERSION}
    EXPORT_NAME FileBrowsing
)
    
target_include_directories(MauiKitFileBrowsing
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/MauiKit/FileBrowsing>")
    
target_include_directories(MauiKitFileBrowsing PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR};${CMAKE_CURRENT_BINARY_DIR};>")

target_link_libraries(
    MauiKitFileBrowsing
       
    PRIVATE
        Qt5::Core
        Qt5::Quick
        Qt5::Qml
        Qt5::Sql
        KF5::I18n
        KF5::ConfigCore
        KF5::KIOCore
        KF5::KIOFileWidgets
        MauiKit
)

install(TARGETS MauiKitFileBrowsing EXPORT FileBrowsingTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
    ${headers} 
    ${CMAKE_CURRENT_BINARY_DIR}/filebrowsing_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MauiKit/FileBrowsing
    COMPONENT Devel)

if (BUILD_SHARED_LIBS)
    add_custom_target(copy_to_bin ALL
        COMMAND ${CMAKE_COMMAND} -E
                make_directory ${CMAKE_BINARY_DIR}/bin/org/mauikit/filebrowsing/
        COMMAND ${CMAKE_COMMAND} -E
                copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/controls ${CMAKE_BINARY_DIR}/bin/org/mauikit/filebrowsing/
        COMMAND ${CMAKE_COMMAND} -E
                copy $<TARGET_FILE:MauiKitFileBrowsing> ${CMAKE_BINARY_DIR}/bin/org/mauikit/filebrowsing/
    )

    install(DIRECTORY controls/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/mauikit/filebrowsing)

    #include(ECMGeneratePriFile)
    #ecm_generate_pri_file(BASE_NAME KQuickImageEditor LIB_NAME KQuickImageEditor DEPS "core qml quick svg" FILENAME_VAR PRI_FILENAME )
    #install(FILES ${PRI_FILENAME}
            #DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

endif()

install(TARGETS MauiKitFileBrowsing DESTINATION ${KDE_INSTALL_QMLDIR}/org/mauikit/filebrowsing )
