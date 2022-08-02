#ifndef INDEX_H
#define INDEX_H

#include "forward_declares.h"
#include <QObject>

class Repository;
struct IndexPrivate;
class Index : public QObject,
              public tbSharedFromThis<Index> {
        Q_OBJECT
    public:
        ~Index();

        struct IndexItem {
        };

        bool hasConflicts();
        void conflictCleanup();

        bool hasChangesFromWorkdir(RepositoryPtr repo);

        QList<IndexItem> items();

    signals:

    protected:
        friend Repository;
        static IndexPtr indexForLgIndex(LGIndexPtr index);

    private:
        explicit Index(QObject* parent = nullptr);
        IndexPrivate* d;
};

#endif // INDEX_H
