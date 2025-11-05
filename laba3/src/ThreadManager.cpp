#include "ThreadManager.h"
#include "Constants.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstdlib>

ThreadContext::ThreadContext() 
    : threadId(0), 
      arrayManager(NULL),
      criticalSection(NULL) {
}

ThreadManager::ThreadManager() 
    : m_threadCount(0),
      m_activeThreadCount(0) {
    m_criticalSection = PlatformSync::createMutex();
}

ThreadManager::~ThreadManager() {
    cleanup();
    PlatformSync::destroyMutex(&m_criticalSection);
}

bool ThreadManager::initialize(int threadCount, ArrayManager* arrayManager) {
    if (threadCount <= 0 || threadCount > Constants::MAX_THREAD_COUNT || arrayManager == NULL) {
        return false;
    }
    
    try {
        m_threadCount = threadCount;
        m_activeThreadCount = threadCount;
        
        m_threadHandles.resize(threadCount);
        m_threadContexts.resize(threadCount);
        m_threadActiveFlags.resize(threadCount, true);
        
        // Initialize thread data
        for (int i = 0; i < threadCount; ++i) {
            m_threadActiveFlags[i] = true;
            
            m_threadContexts[i].threadId = i + 1;
            m_threadContexts[i].arrayManager = arrayManager;
            m_threadContexts[i].criticalSection = &m_criticalSection;
            
            // Create events
            m_threadContexts[i].startEvent = PlatformSync::createEvent(true); // Manual reset
            m_threadContexts[i].suspendEvent = PlatformSync::createEvent(false); // Auto reset
            m_threadContexts[i].continueEvent = PlatformSync::createEvent(false); // Auto reset
            m_threadContexts[i].terminateEvent = PlatformSync::createEvent(false); // Auto reset
            m_threadContexts[i].threadFinished = PlatformSync::createEvent(false); // Auto reset
            
            // Create thread
            m_threadHandles[i] = PlatformSync::createThread(markerThread, &m_threadContexts[i]);
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Initialization error: " << e.what() << std::endl;
        cleanup();
        return false;
    }
}

void ThreadManager::startAllThreads() {
    for (int i = 0; i < m_threadCount; ++i) {
        PlatformSync::setEvent(&m_threadContexts[i].startEvent);
    }
}

void ThreadManager::waitForAllSuspensions() {
    std::vector<SyncEvent*> suspendEvents;
    for (int i = 0; i < m_threadCount; ++i) {
        if (m_threadActiveFlags[i]) {
            suspendEvents.push_back(&m_threadContexts[i].suspendEvent);
        }
    }
    
    for (size_t i = 0; i < suspendEvents.size(); ++i) {
        PlatformSync::waitForEvent(suspendEvents[i], Constants::INFINITE_TIMEOUT);
    }
}

bool ThreadManager::terminateThread(int threadId) {
    int index = threadId - 1;
    if (index < 0 || index >= m_threadCount || !m_threadActiveFlags[index]) {
        return false;
    }
    
    PlatformSync::setEvent(&m_threadContexts[index].terminateEvent);
    return true;
}

void ThreadManager::resumeAllThreads() {
    for (int i = 0; i < m_threadCount; ++i) {
        if (m_threadActiveFlags[i]) {
            PlatformSync::setEvent(&m_threadContexts[i].continueEvent);
        }
    }
}

void ThreadManager::waitForThreadTermination(int threadId) {
    int index = threadId - 1;
    if (index >= 0 && index < m_threadCount) {
        PlatformSync::waitForEvent(&m_threadContexts[index].threadFinished, Constants::INFINITE_TIMEOUT);
        m_threadActiveFlags[index] = false;
        m_activeThreadCount--;
    }
}

int ThreadManager::getActiveThreadCount() const {
    return m_activeThreadCount;
}

bool ThreadManager::isThreadActive(int threadId) const {
    int index = threadId - 1;
    return (index >= 0 && index < m_threadCount && m_threadActiveFlags[index]);
}

void ThreadManager::cleanup() {
    for (int i = 0; i < m_threadCount; ++i) {
        PlatformSync::destroyEvent(&m_threadContexts[i].startEvent);
        PlatformSync::destroyEvent(&m_threadContexts[i].suspendEvent);
        PlatformSync::destroyEvent(&m_threadContexts[i].continueEvent);
        PlatformSync::destroyEvent(&m_threadContexts[i].terminateEvent);
        PlatformSync::destroyEvent(&m_threadContexts[i].threadFinished);
    }
    
    m_threadHandles.clear();
    m_threadContexts.clear();
    m_threadActiveFlags.clear();
    m_threadCount = 0;
    m_activeThreadCount = 0;
}

void* ThreadManager::markerThread(void* parameter) {
    ThreadContext* context = static_cast<ThreadContext*>(parameter);
    if (context == NULL) {
        return NULL;
    }
    
    // Wait for start signal
    PlatformSync::waitForEvent(&context->startEvent, Constants::INFINITE_TIMEOUT);
    
    // Initialize random generator
    srand(context->threadId);
    
    std::vector<int> markedIndices;
    bool continueWorking = true;
    
    while (continueWorking) {
        int randomValue = rand();
        int index = randomValue % context->arrayManager->getSize();
        
        PlatformSync::enterCriticalSection(context->criticalSection);
        
        if (context->arrayManager->getElement(index) == 0) {
            PlatformSync::sleep(Constants::SLEEP_TIME_MS);
            context->arrayManager->setElement(index, context->threadId);
            markedIndices.push_back(index);
            PlatformSync::leaveCriticalSection(context->criticalSection);
            PlatformSync::sleep(Constants::SLEEP_TIME_MS);
        } else {
            std::cout << "Thread " << context->threadId 
                      << ": cannot mark element at index " << index 
                      << ". Marked elements: " << markedIndices.size() << std::endl;
            PlatformSync::leaveCriticalSection(context->criticalSection);
            
            PlatformSync::setEvent(&context->suspendEvent);
            
            // Wait for continue or terminate signal
            bool gotTerminateSignal = false;
            while (true) {
                if (PlatformSync::waitForEvent(&context->terminateEvent, 100)) {
                    gotTerminateSignal = true;
                    break;
                }
                if (PlatformSync::waitForEvent(&context->continueEvent, 100)) {
                    break;
                }
            }
            
            if (gotTerminateSignal) {
                // Termination signal
                PlatformSync::enterCriticalSection(context->criticalSection);
                for (size_t i = 0; i < markedIndices.size(); ++i) {
                    context->arrayManager->setElement(markedIndices[i], 0);
                }
                PlatformSync::leaveCriticalSection(context->criticalSection);
                
                PlatformSync::setEvent(&context->threadFinished);
                continueWorking = false;
            }
        }
    }
    
    return NULL;
}