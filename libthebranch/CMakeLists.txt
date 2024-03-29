project(lib VERSION 1.0.0 LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia Concurrent Network)
find_package(libcontemporary REQUIRED)
find_package(ttexteditor REQUIRED)

set(libgit2_ROOT "C:/Program Files (x86)/libgit2" CACHE PATH "Path to libgit2 on Windows")

cntp_find_pkgconfig(lgit libgit2 REQUIRED
        WIN_FALLBACK_DLL "${libgit2_ROOT}/bin/git2.dll"
        WIN_FALLBACK_IMPLIB "${libgit2_ROOT}/lib/git2.lib"
        WIN_FALLBACK_INCLUDE "${libgit2_ROOT}/include")

set(SOURCES
    libthebranch_global.cpp
    branchservices.cpp

    cred/gitcredentialmanager.cpp

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
    objects/retroactivegitoperation.cpp
    objects/branchuihelper.cpp
    objects/rebase.cpp
    objects/revert.cpp
    objects/stashesmodel.cpp
    objects/stash.cpp
    objects/tag.cpp
    objects/tree.cpp
    objects/blob.cpp
    objects/cherrypick.cpp
    objects/diff.cpp

    popovers/diffpopover.cpp popovers/diffpopover.ui
    popovers/snapinpopover.cpp popovers/snapinpopover.ui
    popovers/newtagpopover.cpp popovers/newtagpopover.ui

    popovers/snapins/snapin.cpp
    popovers/snapins/clonerepositorysnapin.cpp popovers/snapins/clonerepositorysnapin.ui
    popovers/snapins/checkoutsnapin.cpp popovers/snapins/checkoutsnapin.ui
    popovers/snapins/conflictresolutionsnapin.cpp popovers/snapins/conflictresolutionsnapin.ui
    popovers/snapins/commitsnapin.cpp popovers/snapins/commitsnapin.ui
    popovers/snapins/pushsnapin.cpp popovers/snapins/pushsnapin.ui
    popovers/snapins/pullsnapin.cpp popovers/snapins/pullsnapin.ui
    popovers/snapins/newremotesnapin.cpp popovers/snapins/newremotesnapin.ui
    popovers/snapins/mergesnapin.cpp popovers/snapins/mergesnapin.ui
    popovers/snapins/rebasesnapin.cpp popovers/snapins/rebasesnapin.ui
    popovers/snapins/revertsnapin.cpp popovers/snapins/revertsnapin.ui
    popovers/newbranchpopover.cpp popovers/newbranchpopover.ui
    popovers/snapins/stashsavesnapin.cpp popovers/snapins/stashsavesnapin.ui
    popovers/snapins/cherrypicksnapin.cpp popovers/snapins/cherrypicksnapin.ui
    popovers/snapins/commitactionsnapin.cpp popovers/snapins/commitactionsnapin.ui
    popovers/snapins/branchactionsnapin.cpp popovers/snapins/branchactionsnapin.ui

    popovers/snapins/informationrequestsnapin.cpp
    popovers/snapins/sshkeyselectionsnapin.cpp popovers/snapins/sshkeyselectionsnapin.ui
    popovers/snapins/sshcertchecksnapin.cpp popovers/snapins/sshcertchecksnapin.ui
    popovers/snapins/usernamepasswordsnapin.cpp popovers/snapins/usernamepasswordsnapin.ui

    widgets/commitbox.cpp widgets/commitbox.ui
    widgets/repositorybrowser.cpp widgets/repositorybrowser.ui
    widgets/repositorystatusbar.cpp widgets/repositorystatusbar.ui
    widgets/commitbrowser.cpp
    widgets/branchbrowser.cpp
    widgets/remotebrowser.cpp
    widgets/repositorybrowserlist.cpp
    widgets/stashbrowser.cpp

    widgets/commitbrowserwidget.cpp widgets/commitbrowserwidget.ui

    settingspanes/commitspane.cpp settingspanes/commitspane.ui
    settingspanes/accountspane.cpp settingspanes/accountspane.ui

    widgets/conflictresolution/conflictresolutionwidget.cpp
    widgets/conflictresolution/textconflictresolution.cpp

    accounts/accountsmanager.cpp
    accounts/abstractaccount.cpp
    accounts/github/githubhttp.cpp
    accounts/github/githubaccount.cpp
    accounts/github/addgithubaccountpopover.cpp accounts/github/addgithubaccountpopover.ui
    accounts/github/githubitem.cpp
    accounts/github/githubitemdatabase.cpp
    accounts/github/issues/githubissuesapi.cpp
    accounts/github/issues/githubissue.cpp
    accounts/github/issues/githubissuelistcontroller.cpp
    accounts/github/issues/githubissuebrowser.cpp accounts/github/issues/githubissuebrowser.ui
    accounts/github/issues/githubissuemodel.cpp
    accounts/github/issues/githubissueevent.cpp
    accounts/github/issues/githubissuecommentevent.cpp
    accounts/github/issues/events/githubissuecommentbubble.cpp accounts/github/issues/events/githubissuecommentbubble.ui
    accounts/github/issues/events/githubissueeventbubble.cpp accounts/github/issues/events/githubissueeventbubble.ui
    accounts/github/pr/githubcreatepullrequestpopover.cpp accounts/github/pr/githubcreatepullrequestpopover.ui
    accounts/github/pr/githubpullrequestapi.cpp
    accounts/github/pr/githubpullrequestlistcontroller.cpp
    accounts/github/pr/githubpullrequest.cpp
    accounts/github/actions/githubactionsapi.cpp
    accounts/github/actions/githubworkflow.cpp
    accounts/github/actions/githubactionslistcontroller.cpp
    accounts/github/actions/githubactionsbrowser.cpp accounts/github/actions/githubactionsbrowser.ui
    accounts/github/actions/githubactionsworkflowbrowser.cpp accounts/github/actions/githubactionsworkflowbrowser.ui
    accounts/github/actions/githubactionsrunbrowser.cpp accounts/github/actions/githubactionsrunbrowser.ui
    accounts/github/actions/githubactionsmodel.cpp
    accounts/github/actions/githubworkflowrun.cpp
    accounts/github/actions/githubworkflowmodel.cpp
    accounts/github/actions/githubworkflowjob.cpp
    accounts/github/actions/githubworkflowrunmodel.cpp
    accounts/github/actions/githubworkflowjobmodel.cpp
    accounts/github/actions/githubworkflowjobbrowser.cpp accounts/github/actions/githubworkflowjobbrowser.ui
    accounts/github/users/githubuser.cpp

    objects/private/repositoryoperation.cpp
    objects/private/repositorycloneoperation.cpp
    objects/private/informationrequiredcallbackhelper.cpp

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
    objects/libgit/lgtag.cpp
    objects/libgit/lgannotatedtag.cpp
    objects/libgit/lglightweighttag.cpp
    objects/libgit/lgtree.cpp
    objects/libgit/lgremote.cpp
    objects/libgit/lgactiveremote.cpp
    objects/libgit/lgstash.cpp
    objects/libgit/lgobject.cpp
    objects/libgit/lgblob.cpp
    objects/libgit/lgconfig.cpp

    commandpalettes/branchescommandpalette.cpp
)

set(HEADERS
    libthebranch_global.h
    branchservices.h
    objects/icommitresolvable.h
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
    objects/retroactivegitoperation.h
    objects/branchuihelper.h
    objects/rebase.h
    objects/revert.h
    objects/stashesmodel.h
    objects/stash.h
    objects/tree.h
    objects/blob.h
    objects/cherrypick.h
    objects/diff.h
    objects/tag.h
    popovers/snapins/checkoutsnapin.h
    popovers/diffpopover.h
    widgets/repositorybrowser.h
    widgets/commitbrowser.h
    widgets/branchbrowser.h
    widgets/commitbox.h
    widgets/repositorystatusbar.h
    widgets/remotebrowser.h
    widgets/repositorybrowserlist.h
    widgets/stashbrowser.h
    widgets/commitbrowserwidget.h
    objects/forward_declares.h
    popovers/snapinpopover.h
    popovers/snapins/clonerepositorysnapin.h
    popovers/snapins/snapin.h
    popovers/snapins/conflictresolutionsnapin.h
    popovers/snapins/commitsnapin.h
    popovers/snapins/pushsnapin.h
    popovers/snapins/pullsnapin.h
    popovers/snapins/newremotesnapin.h
    popovers/snapins/mergesnapin.h
    popovers/snapins/rebasesnapin.h
    popovers/snapins/revertsnapin.h
    popovers/snapins/stashsavesnapin.h
    popovers/snapins/cherrypicksnapin.h
    popovers/snapins/commitactionsnapin.h
    popovers/snapins/branchactionsnapin.h
    popovers/newbranchpopover.h
    popovers/newtagpopover.h
    settingspanes/commitspane.h
    settingspanes/accountspane.h
    accounts/accountsmanager.h
    accounts/abstractaccount.h
    accounts/github/githubaccount.h
    accounts/github/githubhttp.h
    cred/gitcredentialmanager.h
)

set(PRIVATE_HEADERS
    objects/private/repositoryoperation.h
    objects/private/repositorycloneoperation.h
    objects/private/informationrequiredcallbackhelper.h
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
    objects/libgit/lgtag.h
    objects/libgit/lgannotatedtag.h
    objects/libgit/lglightweighttag.h
    objects/libgit/lgtree.h
    objects/libgit/lgremote.h
    objects/libgit/lgobject.h
    objects/libgit/lgactiveremote.h
    objects/libgit/lgstash.h
    objects/libgit/lgblob.h
    objects/libgit/lgconfig.h
    widgets/conflictresolution/conflictresolutionwidget.h
    widgets/conflictresolution/textconflictresolution.h

    popovers/snapins/informationrequestsnapin.h
    popovers/snapins/sshkeyselectionsnapin.h
    popovers/snapins/sshcertchecksnapin.h
    popovers/snapins/usernamepasswordsnapin.h

    accounts/github/addgithubaccountpopover.h
    accounts/github/githubitem.h
    accounts/github/githubitemdatabase.h
    accounts/github/githubasyncgeneratormodel.h
    accounts/github/issues/githubissuesapi.h
    accounts/github/issues/githubissue.h
    accounts/github/issues/githubissuelistcontroller.h
    accounts/github/issues/githubissuebrowser.h
    accounts/github/issues/githubissuemodel.h
    accounts/github/issues/events/githubissuecommentbubble.h
    accounts/github/issues/events/githubissueeventbubble.h
    accounts/github/issues/githubissueevent.h
    accounts/github/issues/githubissuecommentevent.h
    accounts/github/pr/githubcreatepullrequestpopover.h
    accounts/github/pr/githubpullrequestapi.h
    accounts/github/pr/githubpullrequestlistcontroller.h
    accounts/github/pr/githubpullrequest.h
    accounts/github/actions/githubactionsapi.h
    accounts/github/actions/githubworkflow.h
    accounts/github/actions/githubactionslistcontroller.h
    accounts/github/actions/githubactionsbrowser.h
    accounts/github/actions/githubactionsworkflowbrowser.h
    accounts/github/actions/githubworkflowrun.h
    accounts/github/actions/githubworkflowjob.h
    accounts/github/actions/githubactionsmodel.h
    accounts/github/actions/githubworkflowmodel.h
    accounts/github/actions/githubactionsrunbrowser.h
    accounts/github/actions/githubworkflowrunmodel.h
    accounts/github/actions/githubworkflowjobmodel.h
    accounts/github/actions/githubworkflowjobbrowser.h
    accounts/github/users/githubuser.h

    commandpalettes/branchescommandpalette.h
)

add_library(libthebranch SHARED ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
set_target_properties(libthebranch PROPERTIES
        CNTP_DATA_SUBDIR thebranch/libthebranch)
cntp_init(libthebranch 20)
cntp_translate(libthebranch)
cntp_target_public_headers(libthebranch
    DESTINATION_DIRECTORY libthebranch
    HEADERS ${HEADERS})
set_target_properties(libthebranch PROPERTIES
        OUTPUT_NAME thebranch
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER com.vicr123.libthebranch
        VERSION 1.0.0)

target_link_libraries(libthebranch Qt6::Widgets Qt6::DBus Qt6::Multimedia Qt6::Concurrent Qt6::Network libcontemporary ttexteditor PkgConfig::lgit)
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
