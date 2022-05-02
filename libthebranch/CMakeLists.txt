cmake_minimum_required(VERSION 3.4.0)

project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)
find_package(libcontemporary REQUIRED)

cntp_find_pkgconfig(lgit libgit2 REQUIRED)
#        WIN_FALLBACK_DLL "C:\\Program Files (x86)\\taglib\\bin\\tag.dll"
#        WIN_FALLBACK_IMPLIB "C:\\Program Files (x86)\\taglib\\lib\\tag.lib"
#        WIN_FALLBACK_INCLUDE "C:\\Program Files (x86)\\taglib\\include")

set(SOURCES
    libthebranch_global.cpp
    objects/repository.cpp

    popovers/clonerepositorypopover.cpp popovers/clonerepositorypopover.ui
    widgets/repositorybrowser.cpp widgets/repositorybrowser.ui

    objects/private/repositoryoperation.cpp
    objects/private/repositorycloneoperation.cpp)

set(HEADERS
    libthebranch_global.h
    objects/repository.h
    popovers/clonerepositorypopover.h
    widgets/repositorybrowser.h)

set(PRIVATE_HEADERS
    objects/private/repositoryoperation.h
    objects/private/repositorycloneoperation.h
)

add_library(libthebranch SHARED ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
cntp_init(libthebranch 17)
set_target_properties(libthebranch PROPERTIES
        OUTPUT_NAME thebeat
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.vicr123.libthebranch
        VERSION 1.0.0
        PUBLIC_HEADER "${HEADERS}")

target_link_libraries(libthebranch Qt6::Widgets Qt6::DBus Qt6::Multimedia libcontemporary PkgConfig::lgit)
target_compile_definitions(libthebranch PRIVATE LIBTHEBRANCH_LIBRARY)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/libthebranch.framework/Resources/CMake)
    set(HEADER_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/libthebranch.framework/Headers)
    set(LIBRARY_INSTALL_DIR ../)
ELSE()
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/libthebranch)
    set(HEADER_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/libthebranch)
    set(INCLUDE_INSTALL_DIR ${HEADER_INSTALL_DIR})
    set(LIBRARY_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
ENDIF()

configure_package_config_file(libthebranchConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libthebranchConfig.cmake
        INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
        PATH_VARS HEADER_INSTALL_DIR LIBRARY_INSTALL_DIR)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libthebranchConfig.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(EXPORT libthebranchTargets
        FILE libthebranchTargets.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(TARGETS libthebranch
        EXPORT libthebranchTargets
        PUBLIC_HEADER DESTINATION ${HEADER_INSTALL_DIR}
        INCLUDES DESTINATION ${INCLUDE_INSTALL_DIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR})

cntp_mark_support_library_directory()
