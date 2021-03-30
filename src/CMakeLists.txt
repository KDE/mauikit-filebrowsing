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
    resources.qrc
    ${filebrowsing_QM_LOADER}
)

set(
    headers
    code/fm.h
    code/fmlist.h
    code/placeslist.h
)

set(
    pluginData
    controls/qmldir
)

add_library(
    filebrowsingplugin
    SHARED

    ${sources} ${pluginData}
)

target_link_libraries(
    filebrowsingplugin
       
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

generate_export_header(filebrowsingplugin BASE_NAME FileBrowsing)
install(TARGETS filebrowsingplugin EXPORT FileBrowsingTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

target_include_directories(filebrowsingplugin
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/MauiKit/FileBrowsing>")

if (BUILD_SHARED_LIBS)
    add_custom_target(copy_to_bin ALL
        COMMAND ${CMAKE_COMMAND} -E
                make_directory ${CMAKE_BINARY_DIR}/bin/org/mauikit/filebrowsing/
        COMMAND ${CMAKE_COMMAND} -E
                copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/controls ${CMAKE_BINARY_DIR}/bin/org/mauikit/filebrowsing/
        COMMAND ${CMAKE_COMMAND} -E
                copy $<TARGET_FILE:filebrowsingplugin> ${CMAKE_BINARY_DIR}/bin/org/mauikit/filebrowsing/
    )

    install(DIRECTORY controls/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/mauikit/filebrowsing)

    #include(ECMGeneratePriFile)
    #ecm_generate_pri_file(BASE_NAME KQuickImageEditor LIB_NAME KQuickImageEditor DEPS "core qml quick svg" FILENAME_VAR PRI_FILENAME )
    #install(FILES ${PRI_FILENAME}
            #DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

endif()

install(FILES
    ${headers} 
    ${CMAKE_CURRENT_BINARY_DIR}/filebrowsing_export.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/MauiKit/FileBrowsing
    COMPONENT Devel)

install(TARGETS filebrowsingplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/mauikit/filebrowsing )
