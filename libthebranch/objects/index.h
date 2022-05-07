#ifndef INDEX_H
#define INDEX_H

#include "forward_declares.h"
#include <QObject>

class Repository;
struct IndexPrivate;
class Index : public QObject {
        Q_OBJECT
    public:
        ~Index();

        bool hasConflicts();
        void conflictCleanup();

    signals:

    protected:
        friend Repository;
        static IndexPtr indexForLgIndex(LGIndexPtr index);

    private:
        explicit Index(QObject* parent = nullptr);
        IndexPrivate* d;
};

#endif // INDEX_H
