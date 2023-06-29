#ifndef TAG_H
#define TAG_H

#include "icommitresolvable.h"
#include "libthebranch_global.h"
#include <QObject>

class Repository;
struct TagPrivate;
class LIBTHEBRANCH_EXPORT Tag : public QObject,
                                public ICommitResolvable,
                                public tbSharedFromThis<Tag> {
        Q_OBJECT
    public:
        ~Tag();

        QString name();

    signals:

    protected:
        friend Repository;
        friend Reference;
        static TagPtr tagForLgTag(LGRepositoryPtr repo, LGTagPtr tag);
        LGTagPtr gitTag();

    private:
        explicit Tag(QObject* parent = nullptr);
        TagPrivate* d;

        // ICommitResolvable interface
    public:
        CommitPtr resolveToCommit();
};

#endif // TAG_H
