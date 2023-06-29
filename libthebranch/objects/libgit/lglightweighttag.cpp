#include "lglightweighttag.h"

#include "git2/tag.h"
#include "lgrepository.h"

struct LGLightweightTagPrivate {
        QString fullName;
        QString name;
        LGOidPtr target;
};

LGLightweightTag::LGLightweightTag(QString fullName, LGOidPtr target, QObject* parent) {
    d = new LGLightweightTagPrivate();
    d->fullName = fullName;
    d->name = fullName.replace("refs/tags/", "");
    d->target = target;
}

LGLightweightTag::~LGLightweightTag() {
    delete d;
}

QString LGLightweightTag::name() {
    return d->name;
}

bool LGLightweightTag::deleteTag(LGRepositoryPtr repo) {
    return git_tag_delete(repo->gitRepository(), d->name.toUtf8().constData());
}

LGCommitPtr LGLightweightTag::target(LGRepositoryPtr repo) {
    return repo->lookupCommit(d->target);
}
