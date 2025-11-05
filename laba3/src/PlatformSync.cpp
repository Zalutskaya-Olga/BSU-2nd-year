#include "PlatformSync.h"
#include "Constants.h"
#include <stdexcept>
#include <errno.h>

SyncEvent PlatformSync::createEvent(bool manualReset) {
    SyncEvent event;
    event.signaled = false;
    
    if (pthread_mutex_init(&event.mutex, NULL) != 0) {
        throw std::runtime_error("Failed to create event mutex");
    }
    
    if (pthread_cond_init(&event.condition, NULL) != 0) {
        pthread_mutex_destroy(&event.mutex);
        throw std::runtime_error("Failed to create event condition");
    }
    
    return event;
}

void PlatformSync::destroyEvent(SyncEvent* event) {
    if (event) {
        pthread_cond_destroy(&event->condition);
        pthread_mutex_destroy(&event->mutex);
    }
}

void PlatformSync::setEvent(SyncEvent* event) {
    if (event) {
        pthread_mutex_lock(&event->mutex);
        event->signaled = true;
        pthread_cond_broadcast(&event->condition);
        pthread_mutex_unlock(&event->mutex);
    }
}

void PlatformSync::resetEvent(SyncEvent* event) {
    if (event) {
        pthread_mutex_lock(&event->mutex);
        event->signaled = false;
        pthread_mutex_unlock(&event->mutex);
    }
}

bool PlatformSync::waitForEvent(SyncEvent* event, unsigned int timeoutMs) {
    if (!event) return false;
    
    pthread_mutex_lock(&event->mutex);
    
    if (timeoutMs == Constants::INFINITE_TIMEOUT) {
        while (!event->signaled) {
            pthread_cond_wait(&event->condition, &event->mutex);
        }
    } else {
        struct timespec ts;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        
        ts.tv_sec = tv.tv_sec + (timeoutMs / 1000);
        ts.tv_nsec = (tv.tv_usec * 1000) + ((timeoutMs % 1000) * 1000000);
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }
        
        while (!event->signaled) {
            int result = pthread_cond_timedwait(&event->condition, &event->mutex, &ts);
            if (result == ETIMEDOUT) {
                pthread_mutex_unlock(&event->mutex);
                return false; // Timeout
            }
        }
    }
    
    // For auto-reset events, reset after wait
    event->signaled = false;
    pthread_mutex_unlock(&event->mutex);
    return true;
}

SyncMutex PlatformSync::createMutex() {
    SyncMutex mutex;
    if (pthread_mutex_init(&mutex, NULL) != 0) {
        throw std::runtime_error("Failed to create mutex");
    }
    return mutex;
}

void PlatformSync::destroyMutex(SyncMutex* mutex) {
    if (mutex) {
        pthread_mutex_destroy(mutex);
    }
}

void PlatformSync::enterCriticalSection(SyncMutex* mutex) {
    if (mutex) {
        pthread_mutex_lock(mutex);
    }
}

void PlatformSync::leaveCriticalSection(SyncMutex* mutex) {
    if (mutex) {
        pthread_mutex_unlock(mutex);
    }
}

SyncThread PlatformSync::createThread(void* (*func)(void*), void* param) {
    pthread_t thread;
    if (pthread_create(&thread, NULL, func, param) != 0) {
        throw std::runtime_error("Failed to create thread");
    }
    return thread;
}

void PlatformSync::waitForThread(SyncThread thread) {
    pthread_join(thread, NULL);
}

void PlatformSync::sleep(unsigned int milliseconds) {
    usleep(milliseconds * 1000);
}