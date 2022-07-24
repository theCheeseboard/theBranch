cmake_minimum_required(VERSION 3.4.0)

project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia Concurrent)
find_package(libcontemporary REQUIRED)
find_package(ttexteditor REQUIRED)

cntp_find_pkgconfig(lgit libgit2 REQUIRED
        WIN_FALLBACK_DLL "C:\\Program Files (x86)\\libgit2\\bin\\git2.dll"
        WIN_FALLBACK_IMPLIB "C:\\Program Files (x86)\\libgit2\\lib\\git2.lib"
        WIN_FALLBACK_INCLUDE "C:\\Program Files (x86)\\libgit2\\include")

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
    objects/indexmodel.cpp
    objects/statusitemlistmodel.cpp
    objects/remotesmodel.cpp
    objects/remote.cpp
    objects/repositorymodel.cpp
    objects/branchuihelper.cpp
    objects/rebase.cpp
    objects/stashesmodel.cpp
    objects/stash.cpp

    popovers/clonerepositorypopover.cpp popovers/clonerepositorypopover.ui
    popovers/snapinpopover.cpp popovers/snapinpopover.ui

    popovers/snapins/snapin.cpp
    popovers/snapins/checkoutsnapin.cpp popovers/snapins/checkoutsnapin.ui
    popovers/snapins/conflictresolutionsnapin.cpp popovers/snapins/conflictresolutionsnapin.ui
    popovers/snapins/commitsnapin.cpp popovers/snapins/commitsnapin.ui
    popovers/snapins/pushsnapin.cpp popovers/snapins/pushsnapin.ui
    popovers/snapins/pullsnapin.cpp popovers/snapins/pullsnapin.ui
    popovers/snapins/newremotesnapin.cpp popovers/snapins/newremotesnapin.ui
    popovers/snapins/mergesnapin.cpp popovers/snapins/mergesnapin.ui
    popovers/snapins/rebasesnapin.cpp popovers/snapins/rebasesnapin.ui
    popovers/newbranchpopover.cpp popovers/newbranchpopover.ui
    popovers/snapins/stashsavesnapin.cpp popovers/snapins/stashsavesnapin.ui

    popovers/snapins/informationrequestsnapin.cpp
    popovers/snapins/sshkeyselectionsnapin.cpp popovers/snapins/sshkeyselectionsnapin.ui
    popovers/snapins/sshcertchecksnapin.cpp popovers/snapins/sshcertchecksnapin.ui
    popovers/snapins/usernamepasswordsnapin.cpp popovers/snapins/usernamepasswordsnapin.ui

    widgets/repositorybrowser.cpp widgets/repositorybrowser.ui
    widgets/repositorystatusbar.cpp widgets/repositorystatusbar.ui
    widgets/commitbrowser.cpp
    widgets/branchbrowser.cpp
    widgets/remotebrowser.cpp
    widgets/repositorybrowserlist.cpp
    widgets/stashbrowser.cpp

    settingspanes/commitspane.cpp settingspanes/commitspane.ui
    settingspanes/accountspane.cpp settingspanes/accountspane.ui

    widgets/conflictresolution/conflictresolutionwidget.cpp
    widgets/conflictresolution/textconflictresolution.cpp

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
    objects/libgit/lgtree.cpp
    objects/libgit/lgremote.cpp
    objects/libgit/lgactiveremote.cpp
    objects/libgit/lgstash.cpp
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
    objects/indexmodel.h
    objects/statusitemlistmodel.h
    objects/remotesmodel.h
    objects/remote.h
    objects/repositorymodel.h
    objects/branchuihelper.h
    objects/rebase.h
    objects/stashesmodel.h
    objects/stash.h
    popovers/clonerepositorypopover.h
    popovers/snapins/checkoutsnapin.h
    widgets/repositorybrowser.h
    widgets/commitbrowser.h
    widgets/branchbrowser.h
    widgets/repositorystatusbar.h
    widgets/remotebrowser.h
    widgets/repositorybrowserlist.h
    widgets/stashbrowser.h
    objects/forward_declares.h
    popovers/snapinpopover.h
    popovers/snapins/snapin.h
    popovers/snapins/conflictresolutionsnapin.h
    popovers/snapins/commitsnapin.h
    popovers/snapins/pushsnapin.h
    popovers/snapins/pullsnapin.h
    popovers/snapins/newremotesnapin.h
    popovers/snapins/mergesnapin.h
    popovers/snapins/rebasesnapin.h
    popovers/snapins/stashsavesnapin.h
    popovers/newbranchpopover.h
    settingspanes/commitspane.h
    settingspanes/accountspane.h
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
    objects/libgit/lgtree.h
    objects/libgit/lgremote.h
    objects/libgit/lgactiveremote.h
    objects/libgit/lgstash.h
    widgets/conflictresolution/conflictresolutionwidget.h
    widgets/conflictresolution/textconflictresolution.h

    popovers/snapins/informationrequestsnapin.h
    popovers/snapins/sshkeyselectionsnapin.h
    popovers/snapins/sshcertchecksnapin.h
    popovers/snapins/usernamepasswordsnapin.h

)

add_library(libthebranch SHARED ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
cntp_init(libthebranch 20)
cntp_target_public_headers(libthebranch
    DESTINATION_DIRECTORY libthebranch
    HEADERS ${HEADERS})
set_target_properties(libthebranch PROPERTIES
        OUTPUT_NAME thebranch
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.vicr123.libthebranch
        VERSION 1.0.0)

target_link_libraries(libthebranch Qt6::Widgets Qt6::DBus Qt6::Multimedia Qt6::Concurrent libcontemporary ttexteditor PkgConfig::lgit)
target_compile_definitions(libthebranch PRIVATE LIBTHEBRANCH_LIBRARY)

IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set_target_properties(libthebranch PROPERTIES OUTPUT_NAME libthebranch)
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_CURRENT_BINARY_DIR}/libthebranch.framework/Resources/CMake)
ELSE()
    set(CMAKECONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/libthebranch)
ENDIF()

configure_package_config_file(libthebranchConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/libthebranchConfig.cmake
        INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/libthebranchConfig.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})
install(EXPORT libthebranchTargets
        FILE libthebranchTargets.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR})

install(TARGETS libthebranch
        EXPORT libthebranchTargets
        INCLUDES DESTINATION ${libthebranch_INCLUDE_DIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        FRAMEWORK DESTINATION ${CMAKE_INSTALL_LIBDIR})

cntp_mark_support_library_directory()
