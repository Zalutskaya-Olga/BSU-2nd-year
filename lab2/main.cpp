#include "ThreadManager.h"

int main() {
    try {
        ThreadManager manager;
        manager.execute();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
