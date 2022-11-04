#ifndef LIBTHEBRANCH_GLOBAL_H
#define LIBTHEBRANCH_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(LIBTHEBRANCH_LIBRARY)
    #define LIBTHEBRANCH_EXPORT Q_DECL_EXPORT
#else
    #define LIBTHEBRANCH_EXPORT Q_DECL_IMPORT
#endif

#define LIBTHEBRANCH_TRANSLATOR "thebranch/libthebranch"

namespace theBranch {
    LIBTHEBRANCH_EXPORT void init();
    LIBTHEBRANCH_EXPORT void teardown();
    LIBTHEBRANCH_EXPORT QString gitExecutable();
} // namespace theBranch

#endif // LIBTHEBRANCH_GLOBAL_H
