#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <system_error>
#include <memory>
#include <thread>
#include <chrono>

namespace EmployeeSystem {
    
    constexpr char SERVER_FIFO[] = "/tmp/employee_server_fifo";
    constexpr char CLIENT_FIFO_TEMPLATE[] = "/tmp/employee_client_%d_fifo";
    
    #pragma pack(push, 1)
    struct Employee {
        int32_t id;
        char name[10];
        double hours;
    };

    enum class OperationType : uint8_t {
        READ = 'R',
        WRITE = 'W',
        UNLOCK = 'U',
        EXIT = 'X'
    };

    enum class ResponseStatus : uint8_t {
        SUCCESS = 'S',
        ERROR = 'E',
        LOCKED = 'L',
        NOT_FOUND = 'N'
    };

    struct Request {
        int32_t client_id;
        int32_t employee_id;
        OperationType operation;
        Employee employee;
        uint64_t timestamp;
    };

    struct Response {
        int32_t employee_id;
        ResponseStatus status;
        Employee employee;
        uint64_t timestamp;
    };
    #pragma pack(pop)

    class EmployeeClient {
    private:
        int client_id_;
        std::string client_fifo_path_;
        
        bool wait_for_fifo(const std::string& path, std::ios_base::openmode mode, int max_attempts = 10) {
            for (int i = 0; i < max_attempts; ++i) {
                std::fstream test_file(path, mode);
                if (test_file.is_open()) {
                    return true;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            return false;
        }
        
    public:
        EmployeeClient(int client_id) : client_id_(client_id) {
            char buffer[100];
            snprintf(buffer, sizeof(buffer), CLIENT_FIFO_TEMPLATE, client_id_);
            client_fifo_path_ = buffer;
        }
        
        bool initialize() {
            unlink(client_fifo_path_.c_str());
            
            if (mkfifo(client_fifo_path_.c_str(), 0666) == -1) {
                std::cerr << "Client " << client_id_ << ": Failed to create FIFO - " 
                          << strerror(errno) << std::endl;
                return false;
            }
            
            std::cout << "Client " << client_id_ << ": Created FIFO at " 
                      << client_fifo_path_ << std::endl;
            return true;
        }
        
        Response send_request(const Request& req) {
            Response resp;
            resp.status = ResponseStatus::ERROR;
            resp.employee_id = req.employee_id;
            resp.timestamp = 0;
            
            std::ofstream server_fifo(SERVER_FIFO, std::ios::binary);
            if (!server_fifo) {
                std::cerr << "Client " << client_id_ << ": Cannot open server FIFO for writing" << std::endl;
                std::cerr << "  Path: " << SERVER_FIFO << std::endl;
                std::cerr << "  Error: " << strerror(errno) << std::endl;
                return resp;
            }
            
            server_fifo.write(reinterpret_cast<const char*>(&req), sizeof(Request));
            server_fifo.flush();
            
            if (!server_fifo) {
                std::cerr << "Client " << client_id_ << ": Failed to write request to server FIFO" << std::endl;
                return resp;
            }
            
            server_fifo.close();
            
            auto start_time = std::chrono::steady_clock::now();
            const auto timeout = std::chrono::seconds(5);
            
            while (std::chrono::steady_clock::now() - start_time < timeout) {
                std::ifstream client_fifo(client_fifo_path_, std::ios::binary);
                
                if (client_fifo) {
                    if (client_fifo.read(reinterpret_cast<char*>(&resp), sizeof(Response))) {
                        return resp;
                    }
                    
                    client_fifo.close();
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
            
            std::cerr << "Client " << client_id_ << ": Timeout waiting for response from server" << std::endl;
            std::cerr << "  Expected response in FIFO: " << client_fifo_path_ << std::endl;
            
            struct stat buffer;
            if (stat(client_fifo_path_.c_str(), &buffer) != 0) {
                std::cerr << "  FIFO does not exist!" << std::endl;
            } else {
                std::cerr << "  FIFO exists but no response received" << std::endl;
            }
            
            return resp;
        }
        
        void read_employee() {
            int employee_id;
            std::cout << "\nClient " << client_id_ << " - Enter employee ID to read: ";
            std::cin >> employee_id;
            
            Request req;
            req.client_id = client_id_;
            req.employee_id = employee_id;
            req.operation = OperationType::READ;
            req.timestamp = static_cast<uint64_t>(time(nullptr));
            
            std::cout << "Client " << client_id_ << ": Sending read request for employee " 
                      << employee_id << std::endl;
            
            Response resp = send_request(req);
            
            std::cout << "Client " << client_id_ << ": Received response with status ";
            switch (resp.status) {
                case ResponseStatus::SUCCESS:
                    std::cout << "SUCCESS" << std::endl;
                    std::cout << "\nEmployee record:\n"
                              << "  ID: " << resp.employee.id << "\n"
                              << "  Name: " << resp.employee.name << "\n"
                              << "  Hours: " << resp.employee.hours << std::endl;
                    
                    std::cout << "\nKeep lock? (y/n): ";
                    char choice;
                    std::cin >> choice;
                    
                    if (choice != 'y' && choice != 'Y') {
                        Request unlock_req;
                        unlock_req.client_id = client_id_;
                        unlock_req.employee_id = employee_id;
                        unlock_req.operation = OperationType::UNLOCK;
                        unlock_req.timestamp = static_cast<uint64_t>(time(nullptr));
                        
                        Response unlock_resp = send_request(unlock_req);
                        if (unlock_resp.status == ResponseStatus::SUCCESS) {
                            std::cout << "Lock released." << std::endl;
                        }
                    } else {
                        std::cout << "Lock maintained. Use option 3 to unlock." << std::endl;
                    }
                    break;
                    
                case ResponseStatus::LOCKED:
                    std::cout << "LOCKED - Record is locked by another client" << std::endl;
                    break;
                    
                case ResponseStatus::NOT_FOUND:
                    std::cout << "NOT FOUND - Employee ID " << employee_id << " not found" << std::endl;
                    break;
                    
                case ResponseStatus::ERROR:
                    std::cout << "ERROR - Communication error with server" << std::endl;
                    break;
                    
                default:
                    std::cout << "UNKNOWN (code: " << static_cast<int>(resp.status) << ")" << std::endl;
                    break;
            }
        }
        
        void modify_employee() {
            int employee_id;
            std::cout << "\nClient " << client_id_ << " - Enter employee ID to modify: ";
            std::cin >> employee_id;
            
            Request lock_req;
            lock_req.client_id = client_id_;
            lock_req.employee_id = employee_id;
            lock_req.operation = OperationType::WRITE;
            lock_req.timestamp = static_cast<uint64_t>(time(nullptr));
            
            std::cout << "Client " << client_id_ << ": Acquiring write lock for employee " 
                      << employee_id << std::endl;
            
            Response lock_resp = send_request(lock_req);
            
            if (lock_resp.status != ResponseStatus::SUCCESS) {
                std::cout << "Cannot modify record: ";
                switch (lock_resp.status) {
                    case ResponseStatus::LOCKED:
                        std::cout << "Record is locked by another client" << std::endl;
                        break;
                    case ResponseStatus::NOT_FOUND:
                        std::cout << "Employee not found" << std::endl;
                        break;
                    default:
                        std::cout << "Error accessing record" << std::endl;
                        break;
                }
                return;
            }
            
            std::cout << "\nCurrent record:\n"
                      << "  ID: " << lock_resp.employee.id << "\n"
                      << "  Name: " << lock_resp.employee.name << "\n"
                      << "  Hours: " << lock_resp.employee.hours << std::endl;
            
            Employee modified_emp = lock_resp.employee;
            
            std::cout << "\nEnter new name (max 9 chars, current: '" << modified_emp.name << "'): ";
            std::cin.ignore();
            std::cin.getline(modified_emp.name, sizeof(modified_emp.name));
            
            std::cout << "Enter new hours (current: " << modified_emp.hours << "): ";
            std::cin >> modified_emp.hours;
            
            Request write_req;
            write_req.client_id = client_id_;
            write_req.employee_id = employee_id;
            write_req.operation = OperationType::WRITE;
            write_req.employee = modified_emp;
            write_req.timestamp = static_cast<uint64_t>(time(nullptr));
            
            std::cout << "Client " << client_id_ << ": Sending modification request" << std::endl;
            
            Response write_resp = send_request(write_req);
            
            if (write_resp.status == ResponseStatus::SUCCESS) {
                std::cout << "Record updated successfully!" << std::endl;
            } else {
                std::cout << "Error updating record" << std::endl;
            }
            
            Request unlock_req;
            unlock_req.client_id = client_id_;
            unlock_req.employee_id = employee_id;
            unlock_req.operation = OperationType::UNLOCK;
            unlock_req.timestamp = static_cast<uint64_t>(time(nullptr));
            
            send_request(unlock_req);
            std::cout << "Write lock released." << std::endl;
        }
        
        void unlock_employee() {
            int employee_id;
            std::cout << "\nClient " << client_id_ << " - Enter employee ID to unlock: ";
            std::cin >> employee_id;
            
            Request req;
            req.client_id = client_id_;
            req.employee_id = employee_id;
            req.operation = OperationType::UNLOCK;
            req.timestamp = static_cast<uint64_t>(time(nullptr));
            
            Response resp = send_request(req);
            
            if (resp.status == ResponseStatus::SUCCESS) {
                std::cout << "Employee " << employee_id << " unlocked successfully." << std::endl;
            } else {
                std::cout << "Failed to unlock employee " << employee_id << std::endl;
            }
        }
        
        void run() {
            std::cout << "\n=== Client " << client_id_ << " started ===\n";
            
            while (true) {
                std::cout << "\n=== Client " << client_id_ << " Menu ===\n"
                          << "1. Read employee record\n"
                          << "2. Modify employee record\n"
                          << "3. Unlock employee record\n"
                          << "4. Exit\n"
                          << "Choice: ";
                
                int choice;
                std::cin >> choice;
                
                switch (choice) {
                    case 1:
                        read_employee();
                        break;
                    case 2:
                        modify_employee();
                        break;
                    case 3:
                        unlock_employee();
                        break;
                    case 4:
                        std::cout << "Client " << client_id_ << " exiting...\n";
                        
                        Request exit_req;
                        exit_req.client_id = client_id_;
                        exit_req.employee_id = 0;
                        exit_req.operation = OperationType::EXIT;
                        exit_req.timestamp = static_cast<uint64_t>(time(nullptr));
                        send_request(exit_req);
                        
                        return;
                    default:
                        std::cout << "Invalid choice. Please try again.\n";
                        break;
                }
            }
        }
        
        ~EmployeeClient() {
            unlink(client_fifo_path_.c_str());
            std::cout << "Client " << client_id_ << " FIFO cleaned up." << std::endl;
        }
    };
}

int main(int argc, char* argv[]) {
    using namespace EmployeeSystem;
    
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <client_id>" << std::endl;
        std::cerr << "Example: " << argv[0] << " 1" << std::endl;
        return 1;
    }
    
    try {
        int client_id = std::stoi(argv[1]);
        
        if (client_id < 1 || client_id > 100) {
            std::cerr << "Client ID must be between 1 and 100" << std::endl;
            return 1;
        }
        
        std::cout << "Starting Client " << client_id << "..." << std::endl;
        
        EmployeeClient client(client_id);
        
        if (!client.initialize()) {
            std::cerr << "Client initialization failed" << std::endl;
            return 1;
        }
        
        std::cout << "Waiting for server to be ready..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
        client.run();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}