#include "lgclone.h"

#include <git2.h>

struct LGClonePrivate {
        git_clone_options* options = new git_clone_options();
};

LGClone::LGClone() :
    QObject{nullptr} {
    d = new LGClonePrivate();
    git_clone_options_init(d->options, GIT_CLONE_OPTIONS_VERSION);

    d->options->fetch_opts.callbacks.payload = this;
    d->options->fetch_opts.callbacks.sideband_progress = [](const char* str, int len, void* payload) {
        emit reinterpret_cast<LGClone*>(payload)->statusChanged(QString::fromUtf8(str, len));
        return 0;
    };
    d->options->fetch_opts.callbacks.transfer_progress = [](const git_indexer_progress* stats, void* payload) {
        emit reinterpret_cast<LGClone*>(payload)->progressChanged(stats->received_bytes, stats->received_objects, stats->total_objects);
        return 0;
    };
    d->options->fetch_opts.callbacks.credentials = [](git_credential** out,
                                                       const char* url,
                                                       const char* username_from_url,
                                                       unsigned int allowed_types,
                                                       void* payload) -> int {
        // TODO: Handle more SSH keys
        if (allowed_types & GIT_CREDTYPE_SSH_KEY) {
            QDir sshDir(QDir::home().absoluteFilePath(".ssh"));
            QList<QFileInfo> sshEntries = sshDir.entryInfoList();
            for (QFileInfo sshEntry : sshEntries) {
                if (sshEntry.fileName() == "id_rsa") {
                    // Try this key
                    git_credential_ssh_key_new(out, username_from_url, sshDir.absoluteFilePath("id_rsa.pub").toUtf8().data(), sshDir.absoluteFilePath("id_rsa").toUtf8().data(), "");
                    return 0;
                }
            }
        }
        return GIT_PASSTHROUGH;
    };
}

LGClone::~LGClone() {
    delete d;
}

tPromise<void>* LGClone::clone(QString cloneUrl, QString directory) {
    return TPROMISE_CREATE_NEW_THREAD(void, {
        git_repository* repo;
        if (git_clone(&repo, cloneUrl.toUtf8().data(), directory.toUtf8().data(), d->options) != 0) {
            const git_error* error = git_error_last();
            rej(error->message);
            return;
        }

        git_repository_free(repo);
        res();
    });
}
