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
    objects/commitmodel.cpp
    objects/branchmodel.cpp
    objects/commit.cpp
    objects/branch.cpp
    objects/reference.cpp
    objects/errorresponse.cpp
    objects/merge.cpp
    objects/gitoperation.cpp
    objects/index.cpp

    popovers/clonerepositorypopover.cpp popovers/clonerepositorypopover.ui
    popovers/snapinpopover.cpp popovers/snapinpopover.ui

    popovers/snapins/snapin.cpp
    popovers/snapins/checkoutsnapin.cpp popovers/snapins/checkoutsnapin.ui
    popovers/snapins/conflictresolutionsnapin.cpp popovers/snapins/conflictresolutionsnapin.ui

    widgets/repositorybrowser.cpp widgets/repositorybrowser.ui
    widgets/repositorystatusbar.cpp widgets/repositorystatusbar.ui
    widgets/commitbrowser.cpp
    widgets/branchbrowser.cpp

    objects/private/repositoryoperation.cpp
    objects/private/repositorycloneoperation.cpp

    objects/libgit/lgrepository.cpp
    objects/libgit/lgreference.cpp
    objects/libgit/lgrevwalk.cpp
    objects/libgit/lgoid.cpp
    objects/libgit/lgcommit.cpp
    objects/libgit/lgsignature.cpp
    objects/libgit/lgclone.cpp
    objects/libgit/lgbranch.cpp
    objects/libgit/lgannotatedcommit.cpp
    objects/libgit/lgindex.cpp
)

set(HEADERS
    libthebranch_global.h
    objects/repository.h
    objects/commitmodel.h
    objects/commit.h
    objects/branch.h
    objects/branchmodel.h
    objects/reference.h
    objects/errorresponse.h
    objects/merge.h
    objects/gitoperation.h
    objects/index.h
    popovers/clonerepositorypopover.h
    popovers/snapins/checkoutsnapin.h
    widgets/repositorybrowser.h
    widgets/commitbrowser.h
    widgets/branchbrowser.h
    widgets/repositorystatusbar.h
    objects/forward_declares.h
    popovers/snapinpopover.h
    popovers/snapins/snapin.h
    popovers/snapins/conflictresolutionsnapin.h
)

set(PRIVATE_HEADERS
    objects/private/repositoryoperation.h
    objects/private/repositorycloneoperation.h
    objects/libgit/lgrepository.h
    objects/libgit/lgreference.h
    objects/libgit/lgrevwalk.h
    objects/libgit/lgoid.h
    objects/libgit/lgcommit.h
    objects/libgit/lgsignature.h
    objects/libgit/lgclone.h
    objects/libgit/lgbranch.h
    objects/libgit/lgannotatedcommit.h
    objects/libgit/lgindex.h
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
