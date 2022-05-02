#include "libthebranch_global.h"

#include <QString>
#include <git2.h>
#include <libcontemporary_global.h>

void theBranch::init() {
    git_libgit2_init();
}

void theBranch::teardown() {
    git_libgit2_shutdown();
}

QString theBranch::gitExecutable() {
    static QString gitExecutable = "";
    if (gitExecutable.isEmpty()) {
        QStringList gitExecutables = libContemporaryCommon::searchInPath("git");
        if (!gitExecutables.isEmpty()) gitExecutable = gitExecutables.first();
    }
    return gitExecutable;
}
