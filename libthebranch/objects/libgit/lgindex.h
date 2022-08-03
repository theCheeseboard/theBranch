#ifndef LGINDEX_H
#define LGINDEX_H

#include "../forward_declares.h"
#include <QObject>

struct git_index;
struct LGIndexPrivate;
class QFileInfo;
class LGIndex : public QObject,
                public tbSharedFromThis<LGIndex> {
        Q_OBJECT
    public:
        explicit LGIndex(git_index* git_index);
        explicit LGIndex();
        ~LGIndex();

        struct ::git_index* gitIndex();

        bool write();

        bool hasConflicts();
        void conflictCleanup();

        bool readTree(LGTreePtr tree);
        LGOidPtr writeTree(LGRepositoryPtr repo);

        bool addByPath(QString path);
        bool addAll(QStringList globs);
        bool addBuffer(QFileInfo fileInfo, QString pathspec, QByteArray data);
        bool removeByPath(QString path);

        bool removeAll(QStringList globs);

    signals:

    private:
        LGIndexPrivate* d;
};

#endif // LGINDEX_H
