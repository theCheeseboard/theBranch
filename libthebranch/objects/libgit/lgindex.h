#ifndef LGINDEX_H
#define LGINDEX_H

#include <QObject>

struct git_index;
struct LGIndexPrivate;
class LGIndex : public QObject {
        Q_OBJECT
    public:
        explicit LGIndex(git_index* git_index);
        ~LGIndex();

        git_index* git_index();

    signals:

    private:
        LGIndexPrivate* d;
};

#endif // LGINDEX_H
