#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <set>
#include <mutex>
#include <thread>
#include <atomic>
#include <condition_variable>
#include <algorithm>
#include <system_error>
#include <memory>

#include "employee_types.h"
#include "file_manager.h"
#include "lock_manager.h"
#include "fifo_manager.h"
#include "logger.h"

namespace EmployeeSystem {
    
    class EmployeeServer {
    private:
        FileManager file_manager_;
        LockManager lock_manager_;
        std::atomic<bool> running_{false};
        std::vector<pid_t> client_processes_;
        std::string filename_;
        
    public:
        EmployeeServer(const std::string& filename) 
            : file_manager_(filename), filename_(filename) {}
        
        bool initialize() {
            if (!FIFOManager::create_fifo(SERVER_FIFO)) {
                Logger::log(Logger::Level::ERROR, "Failed to create server FIFO");
                return false;
            }
            
            if (!file_manager_.open()) {
                Logger::log(Logger::Level::ERROR, "Failed to open employee file");
                return false;
            }
            
            Logger::log(Logger::Level::INFO, "Server initialized successfully");
            return true;
        }
        
        void create_employee_file() {
            std::vector<Employee> employees;
            int num_employees;
            
            std::cout << "Enter number of employees: ";
            std::cin >> num_employees;
            std::cin.ignore();
            
            for (int i = 0; i < num_employees; ++i) {
                Employee emp;
                std::cout << "Employee " << (i + 1) << ":\n";
                std::cout << "  ID: ";
                std::cin >> emp.id;
                std::cout << "  Name: ";
                std::cin.ignore();
                std::cin.getline(emp.name, sizeof(emp.name));
                std::cout << "  Hours: ";
                std::cin >> emp.hours;
                
                employees.push_back(emp);
            }
            
            if (file_manager_.write_all(employees)) {
                Logger::log(Logger::Level::INFO, "Employee file created successfully");
            } else {
                Logger::log(Logger::Level::ERROR, "Failed to create employee file");
            }
        }
        
        void display_employee_file() {
            auto employees = file_manager_.read_all();
            std::cout << "\n=== Employee File Contents ===\n";
            for (const auto& emp : employees) {
                std::cout << "ID: " << emp.id 
                          << ", Name: " << emp.name 
                          << ", Hours: " << emp.hours << std::endl;
            }
            std::cout << "=== End of File ===\n\n";
        }
        
        void start_clients(int num_clients) {
            std::string base_dir = "/Users/olga_zalutskaya/Documents/oc/lab5/build";
            
            for (int i = 0; i < num_clients; ++i) {
                int client_id = i + 1;
                
                char command1[1024];
                snprintf(command1, sizeof(command1),
                        "osascript -e 'tell application \"Terminal\"' "
                        "-e 'do script \"cd \\\"%s\\\" && ./client %d\"' "
                        "-e 'end tell'",
                        base_dir.c_str(), client_id);
                
                char command2[1024];
                snprintf(command2, sizeof(command2),
                        "osascript -e 'tell application \"Terminal\"' "
                        "-e 'do script \"\\\"%s/client\\\" %d\"' "
                        "-e 'end tell'",
                        base_dir.c_str(), client_id);
                
                std::cout << "Launching client " << client_id << "..." << std::endl;
                
                int result = system(command1);
                if (result != 0) {
                    std::cout << "Trying alternative command..." << std::endl;
                    system(command2);
                }
                
                usleep(1000000);
            }
            
            std::cout << "\nAll clients should be running now in separate Terminal windows.\n";
        }
        
        void handle_request(const Request& req, Response& resp) {
            resp.employee_id = req.employee_id;
            resp.timestamp = req.timestamp;
            
            auto employees = file_manager_.read_all();
            Employee* emp = file_manager_.find_employee(employees, req.employee_id);
            
            if (!emp && req.operation != OperationType::EXIT) {
                resp.status = ResponseStatus::NOT_FOUND;
                Logger::log(Logger::Level::DEBUG, 
                           "Employee " + std::to_string(req.employee_id) + " not found");
                return;
            }
            
            switch (req.operation) {
                case OperationType::READ:
                    Logger::log(Logger::Level::INFO, 
                               "Client " + std::to_string(req.client_id) + 
                               " reading employee " + std::to_string(req.employee_id));
                    
                    if (lock_manager_.acquire_read_lock(req.employee_id, req.client_id)) {
                        resp.employee = *emp;
                        resp.status = ResponseStatus::SUCCESS;
                        Logger::log(Logger::Level::DEBUG, "Read lock acquired");
                    } else {
                        resp.status = ResponseStatus::LOCKED;
                        Logger::log(Logger::Level::DEBUG, "Read lock denied");
                    }
                    break;
                    
                case OperationType::WRITE:
                    Logger::log(Logger::Level::INFO, 
                               "Client " + std::to_string(req.client_id) + 
                               " writing employee " + std::to_string(req.employee_id));
                    
                    if (lock_manager_.acquire_write_lock(req.employee_id, req.client_id)) {
                        if (req.employee.id != 0) {
                            *emp = req.employee;
                            if (file_manager_.write_all(employees)) {
                                resp.status = ResponseStatus::SUCCESS;
                                Logger::log(Logger::Level::INFO, "Employee updated successfully");
                            } else {
                                resp.status = ResponseStatus::ERROR;
                                Logger::log(Logger::Level::ERROR, "Failed to write to file");
                                lock_manager_.release_write_lock(req.employee_id, req.client_id);
                            }
                        } else {
                            resp.employee = *emp;
                            resp.status = ResponseStatus::SUCCESS;
                            Logger::log(Logger::Level::DEBUG, "Write lock acquired for modification");
                        }
                    } else {
                        resp.status = ResponseStatus::LOCKED;
                        Logger::log(Logger::Level::DEBUG, "Write lock denied");
                    }
                    break;
                    
                case OperationType::UNLOCK:
                    Logger::log(Logger::Level::INFO, 
                               "Client " + std::to_string(req.client_id) + 
                               " unlocking employee " + std::to_string(req.employee_id));
                    
                    lock_manager_.release_read_lock(req.employee_id, req.client_id);
                    lock_manager_.release_write_lock(req.employee_id, req.client_id);
                    resp.status = ResponseStatus::SUCCESS;
                    break;
                    
                case OperationType::EXIT:
                    Logger::log(Logger::Level::INFO, 
                               "Client " + std::to_string(req.client_id) + " exiting");
                    lock_manager_.release_all_locks(req.client_id);
                    resp.status = ResponseStatus::SUCCESS;
                    break;
                    
                default:
                    resp.status = ResponseStatus::ERROR;
                    Logger::log(Logger::Level::WARN, 
                               "Unknown operation from client " + std::to_string(req.client_id));
                    break;
            }
        }
        
        void run() {
            running_ = true;
            
            auto server_fifo = FIFOManager::open_fifo(SERVER_FIFO, std::ios::in | std::ios::binary);
            if (!server_fifo) {
                Logger::log(Logger::Level::ERROR, "Failed to open server FIFO for reading");
                return;
            }
            
            Logger::log(Logger::Level::INFO, "Server started, waiting for requests...");
            
            while (running_) {
                Request req;
                if (server_fifo->read(reinterpret_cast<char*>(&req), sizeof(Request))) {
                    std::string client_fifo_path;
                    char buffer[100];
                    snprintf(buffer, sizeof(buffer), CLIENT_FIFO_TEMPLATE, req.client_id);
                    client_fifo_path = buffer;
                    
                    auto client_fifo = FIFOManager::open_fifo(client_fifo_path, 
                                                            std::ios::out | std::ios::binary);
                    if (client_fifo) {
                        Response resp;
                        handle_request(req, resp);
                        
                        client_fifo->write(reinterpret_cast<const char*>(&resp), sizeof(Response));
                        client_fifo->flush();
                        client_fifo.reset();
                    } else {
                        Logger::log(Logger::Level::ERROR, 
                                   "Failed to open client FIFO: " + client_fifo_path);
                    }
                } else if (!server_fifo->eof()) {
                    Logger::log(Logger::Level::ERROR, "Error reading from FIFO");
                }
                
                for (auto it = client_processes_.begin(); it != client_processes_.end(); ) {
                    int status;
                    pid_t result = waitpid(*it, &status, WNOHANG);
                    if (result > 0) {
                        Logger::log(Logger::Level::INFO, 
                                   "Client process " + std::to_string(*it) + " finished");
                        lock_manager_.release_all_locks(*it);
                        it = client_processes_.erase(it);
                    } else {
                        ++it;
                    }
                }
                
                usleep(10000);
            }
            
            server_fifo.reset();
            FIFOManager::remove_fifo(SERVER_FIFO);
            Logger::log(Logger::Level::INFO, "Server FIFO cleaned up");
        }
        
        void stop() {
            running_ = false;
            
            for (int i = 0; i < client_processes_.size(); ++i) {
                std::cout << "Closing client terminal " << (i + 1) << "..." << std::endl;
            }
            
            for (pid_t pid : client_processes_) {
                int status;
                waitpid(pid, &status, 0);
            }
            
            file_manager_.close();
            Logger::log(Logger::Level::INFO, "Server stopped");
        }
        
        ~EmployeeServer() {
            stop();
        }
    };
}

int main() {
    using namespace EmployeeSystem;
    
    std::string filename;
    std::cout << "Enter employee filename: ";
    std::cin >> filename;
    
    EmployeeServer server(filename);
    
    if (!server.initialize()) {
        Logger::log(Logger::Level::ERROR, "Server initialization failed");
        return 1;
    }
    
    server.create_employee_file();
    server.display_employee_file();
    
    int num_clients;
    std::cout << "Enter number of clients: ";
    std::cin >> num_clients;
    
    if (num_clients > 0) {
        server.start_clients(num_clients);
        
        std::thread server_thread([&server]() { server.run(); });
        
        std::cout << "\nServer running. Clients started in separate terminals.\n";
        std::cout << "Press 'q' and Enter to stop server and close all clients...\n";
        std::string command;
        while (std::cin >> command) {
            if (command == "q" || command == "quit") {
                break;
            }
        }
        
        server.stop();
        server_thread.join();
        
        std::cout << "\nFinal file contents:\n";
        server.display_employee_file();
    }
    
    return 0;
}