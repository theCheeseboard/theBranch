#ifndef LGINDEX_H
#define LGINDEX_H

#include <QObject>

struct git_index;
struct LGIndexPrivate;
class LGIndex : public QObject {
        Q_OBJECT
    public:
        explicit LGIndex(git_index* git_index);
        explicit LGIndex();
        ~LGIndex();

        git_index* git_index();

        bool hasConflicts();
        void conflictCleanup();

        void addAll(QStringList globs);

    signals:

    private:
        LGIndexPrivate* d;
};

#endif // LGINDEX_H
