
#include <QMutex>
#include <QHash>

#include <dlfcn.h>
#include <malloc.h>

#ifndef QT_NO_DEBUG

QMutex * mutex = NULL;
QHash<void *, size_t> ptrTable;
size_t allocMemory = 0;

void * (* __os_malloc) (size_t size) = NULL;
void * (* __os_realloc) (void* ptr, size_t size) = NULL;
void   (* __os_free) (void *ptr) = NULL;

int malloc_hook_active = 0;

void * my_malloc_hook (size_t size, const void *caller)
{
    QMutexLocker locker(mutex);

    void *result;

    // deactivate hooks for logging
    malloc_hook_active = 0;

    result = malloc(size);

    // do logging
    printf ("malloc (%u) returns %p\n", (unsigned int) size, result);
    allocMemory += size;
    ptrTable.insert(result, size);

    // reactivate hooks
    malloc_hook_active = 1;

    return result;
}

void * my_realloc_hook (void * ptr, size_t size, const void *caller)
{
    QMutexLocker locker(mutex);

    void *result;

    // deactivate hooks for logging
    malloc_hook_active = 0;

    result = realloc(ptr, size);

    // do logging
    printf ("realloc (%u) for pointer %p returns %p\n", (unsigned int) size, ptr, result);
    if (NULL != result) {
        if (NULL == ptr) {      // like malloc
            allocMemory += size;
            ptrTable.insert(result, size);
        }
        else {
            if (0 == size) {    // like free
                size = ptrTable.value(ptr);
                allocMemory -= size;
                ptrTable.remove(ptr);
            }
            else {
                size_t oldSize = ptrTable.value(ptr);
                allocMemory += size;
                allocMemory -= oldSize;
                ptrTable.insert(result, size);
                if (ptr != result) {    // reallocation
                    ptrTable.remove(ptr);
                }
            }
        }
    }

    // reactivate hooks
    malloc_hook_active = 1;

    return result;
}

void my_free_hook (void * ptr, const void *caller)
{
    QMutexLocker locker(mutex);

    // deactivate hooks for logging
    malloc_hook_active = 0;

    free (ptr);

    printf ("freed pointer %p\n", ptr);
    size_t size = ptrTable.value(ptr);
    allocMemory -= size;
    ptrTable.remove(ptr);

    // reactivate hooks
    malloc_hook_active = 1;
}

void * malloc (size_t size)
{
    if (NULL == __os_malloc) {
       __os_malloc = (void * (*) (size_t)) dlsym(RTLD_NEXT, "malloc");
    }
    if (malloc_hook_active) {
        void *caller = __builtin_return_address(0);
        return my_malloc_hook(size, caller);
    }
    return __os_malloc(size);
}

void * realloc (void* ptr, size_t size)
{
    if (NULL == __os_realloc) {
       __os_realloc = (void * (*) (void* ptr, size_t size)) dlsym(RTLD_NEXT, "realloc");
    }
    if (malloc_hook_active) {
        void *caller = __builtin_return_address(0);
        return my_realloc_hook(ptr, size, caller);
    }
    return __os_realloc(ptr, size);
}

void free (void * ptr)
{
    if (NULL == __os_free) {
       __os_free = (void (*) (void *)) dlsym(RTLD_NEXT, "free");
    }
    if (malloc_hook_active) {
        void *caller = __builtin_return_address(0);
        return my_free_hook(ptr, caller);
    }
    return __os_free(ptr);
}


void UnhookAlloc ()
{
    malloc_hook_active = 0;
    if (mutex) {
        delete mutex;
    }
}


void HookAlloc ()
{
    malloc_hook_active = 0;
    atexit (UnhookAlloc);

    mutex = new QMutex();
    allocMemory = 0;
    malloc_hook_active = 1;
}

#endif

