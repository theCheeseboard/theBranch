#include "lgclone.h"

#include "../private/informationrequiredcallbackhelper.h"
#include "branchservices.h"
#include <QCoroFuture>
#include <git2.h>

struct LGClonePrivate {
        InformationRequiredCallbackHelper informationRequiredCallbackHelper;
        git_clone_options* options = new git_clone_options();
};

LGClone::LGClone() :
    QObject{nullptr} {
    d = new LGClonePrivate();
    git_clone_options_init(d->options, GIT_CLONE_OPTIONS_VERSION);

    d->informationRequiredCallbackHelper.setPassthroughPayload(this);

    // Ensure cred manager is available
    BranchServices::cred();

    d->options->fetch_opts.callbacks.payload = &d->informationRequiredCallbackHelper;
    d->options->fetch_opts.callbacks.sideband_progress = [](const char* str, int len, void* payload) {
        emit reinterpret_cast<LGClone*>(reinterpret_cast<InformationRequiredCallbackHelper*>(payload)->passthroughPayload())->statusChanged(QString::fromUtf8(str, len));
        return 0;
    };
    d->options->fetch_opts.callbacks.transfer_progress = [](const git_indexer_progress* stats, void* payload) {
        emit reinterpret_cast<LGClone*>(reinterpret_cast<InformationRequiredCallbackHelper*>(payload)->passthroughPayload())->progressChanged(stats->received_bytes, stats->received_objects, stats->total_objects);
        return 0;
    };
    d->options->fetch_opts.callbacks.credentials = d->informationRequiredCallbackHelper.acquireCredentialCallback();
}

LGClone::~LGClone() {
    delete d;
}

QCoro::Task<> LGClone::clone(QString cloneUrl, QString directory) {
    auto ok = co_await QtConcurrent::run([this](QString cloneUrl, QString directory) {
        git_repository* repo;
        if (git_clone(&repo, cloneUrl.toUtf8().data(), directory.toUtf8().data(), d->options) != 0) {
            const git_error* error = git_error_last();
            return false;
        }

        git_repository_free(repo);
        return true;
    },
        cloneUrl, directory);

    if (!ok) throw QException();
}

void LGClone::setInformationRequiredCallback(InformationRequiredCallback callback) {
    d->informationRequiredCallbackHelper.setInformationRequiredCallback(callback);
}
