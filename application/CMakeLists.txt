cmake_minimum_required(VERSION 3.21.0)

project(thebranch VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets DBus Sql PrintSupport)
find_package(libcontemporary REQUIRED)

set(SOURCES
        main.cpp
        mainwindow.cpp mainwindow.ui
        printcontroller.cpp
)

set(HEADERS
        mainwindow.h
        printcontroller.h
)

# Find Apple lproj files
set(LPROJ_SOURCES)
file(GLOB LPROJ_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/translations/apple-lproj/*.lproj)
FOREACH(DIR ${LPROJ_DIRS})
    cmake_path(GET DIR FILENAME DIR_NAME)
    file(GLOB LPROJ_FILES ${DIR}/*.strings)
    FOREACH(FILE ${LPROJ_FILES})
        list(APPEND LPROJ_SOURCES ${FILE})
        set_source_files_properties(${FILE} PROPERTIES
                MACOSX_PACKAGE_LOCATION Resources/${DIR_NAME})
    ENDFOREACH()
ENDFOREACH()
set(SOURCES ${SOURCES} ${LPROJ_SOURCES})

add_executable(thebranch ${SOURCES} ${HEADERS})

set_target_properties(thebranch PROPERTIES
        MACOSX_BUNDLE TRUE
#        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        MACOSX_BUNDLE_GUI_IDENTIFIER com.vicr123.thebranch
        WIN32_EXECUTABLE TRUE
        CNTP_DATA_SUBDIR thebranch)

cntp_init(thebranch 20)
cntp_translate(thebranch)
cntp_target_name(thebranch "theBranch")
cntp_app_icon(thebranch
    BASEICON icons/baseicon.svg
    BASEICON_MAC icons/baseicon-mac.svg)
cntp_dmg(thebranch app-dmg-background.svg)
cntp_target_desktop_file(thebranch
    DESKTOP_FILE com.vicr123.thebranch.desktop
    BLUEPRINT_DESKTOP_FILE com.vicr123.thebranch_blueprint.desktop)
cntp_defaults_file(thebranch
    DEFAULTS_FILE thebranch.conf)

target_link_libraries(thebranch Qt6::Widgets Qt6::DBus Qt6::Sql Qt6::PrintSupport libcontemporary libthebranch)
target_include_directories(thebranch PUBLIC ../libthebranch/)

install(TARGETS thebranch
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        BUNDLE DESTINATION /Applications)
