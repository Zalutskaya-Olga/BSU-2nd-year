#include "../include/ArrayManager.h"
#include "../include/ThreadManager.h"
#include "../include/Constants.h"
#include <iostream>
#include <stdexcept>
#include <limits>

class Application {
public:
    Application() : m_arrayManager(NULL) {}
    
    ~Application() {
        if (m_arrayManager != NULL) {
            delete m_arrayManager;
        }
    }
    
    void run() {
        try {
            initialize();
            mainLoop();
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

private:
    void initialize() {
        int arraySize = readArraySize();
        m_arrayManager = new ArrayManager(arraySize);
        
        int threadCount = readThreadCount();
        if (!m_threadManager.initialize(threadCount, m_arrayManager)) {
            throw std::runtime_error("Failed to initialize thread manager");
        }
        
        m_threadManager.startAllThreads();
        std::cout << "All threads started. Beginning work..." << std::endl;
    }

    void mainLoop() {
        while (m_threadManager.getActiveThreadCount() > 0) {
            m_threadManager.waitForAllSuspensions();
            
            std::cout << "\nCurrent array state:" << std::endl;
            m_arrayManager->printArray();
            
            int threadToTerminate = readThreadToTerminate();
            if (!processThreadTermination(threadToTerminate)) {
                continue;
            }
            
            std::cout << "\nArray state after terminating thread " << threadToTerminate << ":" << std::endl;
            m_arrayManager->printArray();
            
            m_threadManager.resumeAllThreads();
            std::cout << "Active threads remaining: " << m_threadManager.getActiveThreadCount() << std::endl;
        }
        
        std::cout << "\nAll threads completed. Program finished." << std::endl;
    }

    int readArraySize() {
        int size = 0;
        while (true) {
            std::cout << "Enter array size: ";
            if (std::cin >> size) {
                if (size >= Constants::MIN_ARRAY_SIZE && size <= Constants::MAX_ARRAY_SIZE) {
                    break;
                } else {
                    std::cout << "Array size must be between " << Constants::MIN_ARRAY_SIZE 
                              << " and " << Constants::MAX_ARRAY_SIZE << std::endl;
                }
            } else {
                std::cout << "Invalid input. Please enter a number." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        return size;
    }

    int readThreadCount() {
        int count = 0;
        while (true) {
            std::cout << "Enter number of marker threads: ";
            if (std::cin >> count) {
                if (count > 0 && count <= Constants::MAX_THREAD_COUNT) {
                    break;
                } else {
                    std::cout << "Thread count must be between 1 and " 
                              << Constants::MAX_THREAD_COUNT << std::endl;
                }
            } else {
                std::cout << "Invalid input. Please enter a number." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        return count;
    }

    int readThreadToTerminate() {
        int threadId = 0;
        while (true) {
            std::cout << "\nEnter thread ID to terminate (1-" << m_threadManager.getActiveThreadCount() << "): ";
            if (std::cin >> threadId) {
                if (m_threadManager.isThreadActive(threadId)) {
                    break;
                } else {
                    std::cout << "Invalid thread ID or thread is not active." << std::endl;
                }
            } else {
                std::cout << "Invalid input. Please enter a number." << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            }
        }
        return threadId;
    }

    bool processThreadTermination(int threadId) {
        if (!m_threadManager.terminateThread(threadId)) {
            std::cout << "Failed to terminate thread " << threadId << std::endl;
            return false;
        }
        
        m_threadManager.waitForThreadTermination(threadId);
        return true;
    }

    ArrayManager* m_arrayManager;
    ThreadManager m_threadManager;
};

int main() {
    Application app;
    app.run();
    return 0;
}