#ifndef LGINDEX_H
#define LGINDEX_H

#include "../forward_declares.h"
#include <QObject>

struct git_index;
struct LGIndexPrivate;
class QFileInfo;
class LGIndex : public QObject {
        Q_OBJECT
    public:
        explicit LGIndex(git_index* git_index);
        explicit LGIndex();
        ~LGIndex();

        struct ::git_index* gitIndex();

        void write();

        bool hasConflicts();
        void conflictCleanup();

        bool readTree(LGTreePtr tree);
        LGOidPtr writeTree(LGRepositoryPtr repo);

        bool addAll(QStringList globs);
        bool addBuffer(QFileInfo fileInfo, QString pathspec, QByteArray data);

        bool removeAll(QStringList globs);

    signals:

    private:
        LGIndexPrivate* d;
};

#endif // LGINDEX_H
