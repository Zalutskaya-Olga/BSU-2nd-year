#include "employee_types.h"
#include "file_manager.h"
#include "lock_manager.h"
#include "logger.h"
#include "fifo_manager.h"
#include <gtest/gtest.h>
#include <filesystem>
#include <thread>
#include <atomic>

namespace EmployeeSystem {

class IntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_filename_ = "integration_test.dat";
        test_fifo_path_ = "/tmp/integration_test_fifo";
        
        std::filesystem::remove(test_filename_);
        std::filesystem::remove(test_fifo_path_);
    }
    
    void TearDown() override {
        std::filesystem::remove(test_filename_);
        std::filesystem::remove(test_fifo_path_);
    }
    
    std::string test_filename_;
    std::string test_fifo_path_;
};

TEST_F(IntegrationTest, CompleteWorkflow) {
    FileManager file_manager(test_filename_);
    EXPECT_TRUE(file_manager.open());
    
    std::vector<Employee> employees = {
        Employee(1, "Alice", 40.0),
        Employee(2, "Bob", 35.5),
        Employee(3, "Charlie", 42.0)
    };
    
    EXPECT_TRUE(file_manager.write_all(employees));
    
    auto read_employees = file_manager.read_all();
    ASSERT_EQ(read_employees.size(), 3);
    EXPECT_TRUE(employees[0] == read_employees[0]);
    
    Employee* found = file_manager.find_employee(read_employees, 2);
    ASSERT_NE(found, nullptr);
    EXPECT_EQ(found->id, 2);
    EXPECT_STREQ(found->name, "Bob");
    
    found->hours = 38.0;
    strncpy(found->name, "Robert", sizeof(found->name) - 1);
    
    EXPECT_TRUE(file_manager.write_all(read_employees));
    
    read_employees = file_manager.read_all();
    found = file_manager.find_employee(read_employees, 2);
    ASSERT_NE(found, nullptr);
    EXPECT_DOUBLE_EQ(found->hours, 38.0);
    EXPECT_STREQ(found->name, "Robert");
}

TEST_F(IntegrationTest, ConcurrentFileAccessWithLocks) {
    FileManager file_manager(test_filename_);
    EXPECT_TRUE(file_manager.open());
    
    std::vector<Employee> initial_employees = {
        Employee(1, "Initial", 0.0)
    };
    EXPECT_TRUE(file_manager.write_all(initial_employees));
    
    LockManager lock_manager;
    std::atomic<int> successful_reads{0};
    std::atomic<int> successful_writes{0};
    
    constexpr int NUM_THREADS = 5;
    constexpr int NUM_ITERATIONS = 20;
    
    std::vector<std::thread> threads;
    
    for (int thread_id = 0; thread_id < NUM_THREADS; ++thread_id) {
        threads.emplace_back([&, thread_id]() {
            int client_id = thread_id + 1;
            
            for (int i = 0; i < NUM_ITERATIONS; ++i) {
                if (lock_manager.acquire_read_lock(1, client_id)) {
                    auto employees = file_manager.read_all();
                    if (!employees.empty()) {
                        successful_reads++;
                    }
                    lock_manager.release_read_lock(1, client_id);
                }
                
                if (lock_manager.acquire_write_lock(1, client_id)) {
                    auto employees = file_manager.read_all();
                    if (!employees.empty()) {
                        employees[0].hours += 1.0;
                        if (file_manager.write_all(employees)) {
                            successful_writes++;
                        }
                    }
                    lock_manager.release_write_lock(1, client_id);
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto final_employees = file_manager.read_all();
    ASSERT_EQ(final_employees.size(), 1);
    
    EXPECT_GE(final_employees[0].hours, initial_employees[0].hours);
    
    Logger::info("Concurrent test: " + 
                std::to_string(successful_reads) + " successful reads, " +
                std::to_string(successful_writes) + " successful writes");
}

TEST_F(IntegrationTest, RequestResponseSimulation) {
    FIFOManager::create_fifo(test_fifo_path_);
    
    std::atomic<bool> server_running{true};
    std::thread server_thread([&]() {
        FileManager file_manager(test_filename_);
        file_manager.open();
        
        std::vector<Employee> initial_employees = {
            Employee(100, "ServerTest", 50.0)
        };
        file_manager.write_all(initial_employees);
        
        LockManager lock_manager;
        
        while (server_running) {
            auto stream = FIFOManager::open_fifo(test_fifo_path_, 
                                               std::ios::in | std::ios::binary);
            if (!stream) {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                continue;
            }
            
            Request req;
            if (stream->read(reinterpret_cast<char*>(&req), sizeof(Request))) {
                Response resp;
                resp.employee_id = req.employee_id;
                resp.timestamp = req.timestamp;
                
                if (req.operation == OperationType::READ) {
                    if (lock_manager.acquire_read_lock(req.employee_id, req.client_id)) {
                        auto employees = file_manager.read_all();
                        Employee* emp = file_manager.find_employee(employees, req.employee_id);
                        if (emp) {
                            resp.employee = *emp;
                            resp.status = ResponseStatus::SUCCESS;
                        } else {
                            resp.status = ResponseStatus::NOT_FOUND;
                        }
                    } else {
                        resp.status = ResponseStatus::LOCKED;
                    }
                } else {
                    resp.status = ResponseStatus::ERROR;
                }
                
                Logger::debug("Server processed request from client " + 
                            std::to_string(req.client_id) + 
                            " for employee " + std::to_string(req.employee_id));
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
    
    std::thread client_thread([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        
        Request req;
        req.client_id = 999;
        req.employee_id = 100;
        req.operation = OperationType::READ;
        req.timestamp = static_cast<uint64_t>(time(nullptr));
        
        auto stream = FIFOManager::open_fifo(test_fifo_path_, 
                                           std::ios::out | std::ios::binary);
        ASSERT_NE(stream, nullptr);
        
        stream->write(reinterpret_cast<char*>(&req), sizeof(Request));
        stream->flush();
        
        Logger::debug("Client sent read request for employee " + 
                     std::to_string(req.employee_id));
    });
    
    client_thread.join();
    
    server_running = false;
    server_thread.join();
    
    FileManager verify_manager(test_filename_);
    EXPECT_TRUE(verify_manager.open());
    auto employees = verify_manager.read_all();
    EXPECT_FALSE(employees.empty());
}

TEST_F(IntegrationTest, ErrorHandlingAndRecovery) {
    Logger::info("Starting error handling test");
    
    FileManager file_manager("nonexistent_file.dat");
    EXPECT_TRUE(file_manager.open());
    
    auto employees = file_manager.read_all();
    EXPECT_TRUE(employees.empty());
    
    std::vector<Employee> test_data = {
        Employee(1, "Test1", 10.0),
        Employee(2, "Test2", 20.0)
    };
    EXPECT_TRUE(file_manager.write_all(test_data));
    
    std::ofstream corrupt_file("nonexistent_file.dat", std::ios::binary);
    corrupt_file.write("CORRUPTED", 9);
    corrupt_file.close();
    
    employees = file_manager.read_all();
    EXPECT_TRUE(file_manager.write_all(test_data));
    employees = file_manager.read_all();
    ASSERT_EQ(employees.size(), 2);
    EXPECT_TRUE(test_data[0] == employees[0]);
    
    Logger::info("Error handling test completed");
}

TEST_F(IntegrationTest, LoggerIntegration) {
    std::stringstream test_buffer;
    auto old_cout_buf = std::cout.rdbuf();
    std::cout.rdbuf(test_buffer.rdbuf());
    
    Logger::debug("Debug message from integration test");
    Logger::info("Info message from integration test");
    Logger::warn("Warning message from integration test");
    Logger::error("Error message from integration test");
    
    std::cout.rdbuf(old_cout_buf);
    
    std::string log_output = test_buffer.str();
    
    EXPECT_NE(log_output.find("Debug message from integration test"), std::string::npos);
    EXPECT_NE(log_output.find("Info message from integration test"), std::string::npos);
    EXPECT_NE(log_output.find("Warning message from integration test"), std::string::npos);
    EXPECT_NE(log_output.find("Error message from integration test"), std::string::npos);
    
    EXPECT_NE(log_output.find("["), std::string::npos);
    EXPECT_NE(log_output.find("]"), std::string::npos);
}

} 