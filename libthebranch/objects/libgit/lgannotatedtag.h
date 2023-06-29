#ifndef LGANNOTATEDTAG_H
#define LGANNOTATEDTAG_H

#include "lgtag.h"

struct git_tag;
struct LGAnnotatedTagPrivate;
class LGAnnotatedTag : public LGTag,
                       public tbSharedFromThis<LGAnnotatedTag> {
        Q_OBJECT
    public:
        explicit LGAnnotatedTag(git_tag* git_tag);
        ~LGAnnotatedTag();

        struct ::git_tag* gitTag();
        struct ::git_tag* takeGitTag();

        LGAnnotatedTagPtr dup();

        QString name();
        LGCommitPtr target(LGRepositoryPtr repo);
        bool deleteTag(LGRepositoryPtr repo);

    signals:

    private:
        LGAnnotatedTagPrivate* d;
};

#endif // LGANNOTATEDTAG_H
