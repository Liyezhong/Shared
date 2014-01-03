#ifndef GLOBAL_ALLOCHOOK_H
#define GLOBAL_ALLOCHOOK_H

#ifdef QT_NO_DEBUG
    void HookAlloc() {;}
#else
    void HookAlloc();
#endif

#endif // GLOBAL_ALLOCHOOK_H
