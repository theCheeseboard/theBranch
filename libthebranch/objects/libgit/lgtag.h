#ifndef LGTAG_H
#define LGTAG_H

#include "../forward_declares.h"
#include <QObject>

struct git_tag;
struct LGTagPrivate;
class LGTag : public QObject,
              public tbSharedFromThis<LGTag> {
        Q_OBJECT
    public:
        explicit LGTag(git_tag* git_tag);
        ~LGTag();

        struct ::git_tag* gitTag();
        struct ::git_tag* takeGitTag();

        LGTagPtr dup();

        QString name();

        bool deleteTag(LGRepositoryPtr repo);

    signals:

    private:
        LGTagPrivate* d;
};

#endif // LGTAG_H
