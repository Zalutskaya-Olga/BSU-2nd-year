#include "fifo_manager.h"
#include <sys/stat.h>
#include <unistd.h>

namespace EmployeeSystem {

    bool FIFOManager::create_fifo(const std::string& path) {
        ::unlink(path.c_str());
        return ::mkfifo(path.c_str(), 0666) == 0;
    }
    
    bool FIFOManager::remove_fifo(const std::string& path) {
        return ::unlink(path.c_str()) == 0;
    }
    
    std::unique_ptr<std::fstream> FIFOManager::open_fifo(const std::string& path, 
                                                       std::ios_base::openmode mode) {
        auto stream = std::make_unique<std::fstream>(path, mode);
        return stream->is_open() ? std::move(stream) : nullptr;
    }

} 