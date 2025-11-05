#ifndef PLATFORM_SYNC_H
#define PLATFORM_SYNC_H

#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

// Platform-independent synchronization types
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t condition;
    bool signaled;
} SyncEvent;

typedef pthread_t SyncThread;
typedef pthread_mutex_t SyncMutex;

// Platform-independent functions
class PlatformSync {
public:
    static SyncEvent createEvent(bool manualReset = false);
    static void destroyEvent(SyncEvent* event);
    static void setEvent(SyncEvent* event);
    static void resetEvent(SyncEvent* event);
    static bool waitForEvent(SyncEvent* event, unsigned int timeoutMs = 0xFFFFFFFF);
    
    static SyncMutex createMutex();
    static void destroyMutex(SyncMutex* mutex);
    static void enterCriticalSection(SyncMutex* mutex);
    static void leaveCriticalSection(SyncMutex* mutex);
    
    static SyncThread createThread(void* (*func)(void*), void* param);
    static void waitForThread(SyncThread thread);
    static void sleep(unsigned int milliseconds);
};

#endif