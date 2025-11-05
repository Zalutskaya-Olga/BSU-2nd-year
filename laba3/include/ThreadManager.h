#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "ArrayManager.h"
#include "PlatformSync.h"
#include <vector>

struct ThreadContext {
    int threadId;
    ArrayManager* arrayManager;
    SyncEvent startEvent;
    SyncEvent suspendEvent; 
    SyncEvent continueEvent;
    SyncEvent terminateEvent;
    SyncEvent threadFinished;
    SyncMutex* criticalSection;
    
    ThreadContext();
};

class ThreadManager {
public:
    ThreadManager();
    ~ThreadManager();
    
    bool initialize(int threadCount, ArrayManager* arrayManager);
    void startAllThreads();
    void waitForAllSuspensions();
    bool terminateThread(int threadId);
    void resumeAllThreads();
    void waitForThreadTermination(int threadId);
    int getActiveThreadCount() const;
    bool isThreadActive(int threadId) const;
    void cleanup();
    
private:
    static void* markerThread(void* parameter);
    
    std::vector<SyncThread> m_threadHandles;
    std::vector<ThreadContext> m_threadContexts;
    std::vector<bool> m_threadActiveFlags;
    int m_threadCount;
    int m_activeThreadCount;
    SyncMutex m_criticalSection;
    
    // Prevent copying
    ThreadManager(const ThreadManager&);
    ThreadManager& operator=(const ThreadManager&);
};

#endif