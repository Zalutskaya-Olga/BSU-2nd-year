#include "file_manager.h"
#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <atomic>


namespace EmployeeSystem {

class FileManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_filename_ = "test_employees.dat";
        manager_ = std::make_unique<FileManager>(test_filename_);
    }
    
    void TearDown() override {
        manager_->close();
        std::filesystem::remove(test_filename_);
        std::filesystem::remove(test_filename_ + ".tmp");
    }
    
    void CreateTestFile(const std::vector<Employee>& employees) {
        std::ofstream file(test_filename_, std::ios::binary);
        if (!employees.empty()) {
            file.write(reinterpret_cast<const char*>(employees.data()),
                      employees.size() * sizeof(Employee));
        }
        file.close();
    }
    
    std::string test_filename_;
    std::unique_ptr<FileManager> manager_;
};

TEST_F(FileManagerTest, OpenNewFile) {
    EXPECT_TRUE(manager_->open());
}

TEST_F(FileManagerTest, OpenExistingFile) {
    std::vector<Employee> employees = {
        Employee(1, "John", 40.0),
        Employee(2, "Jane", 35.5)
    };
    CreateTestFile(employees);
    
    EXPECT_TRUE(manager_->open());
}

TEST_F(FileManagerTest, ReadAllEmptyFile) {
    manager_->open();
    auto employees = manager_->read_all();
    EXPECT_TRUE(employees.empty());
}

TEST_F(FileManagerTest, ReadAllWithData) {
    std::vector<Employee> expected = {
        Employee(1, "John", 40.0),
        Employee(2, "Jane", 35.5),
        Employee(3, "Bob", 42.0)
    };
    CreateTestFile(expected);
    
    manager_->open();
    auto actual = manager_->read_all();
    
    ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_TRUE(expected[i] == actual[i]);
    }
}

TEST_F(FileManagerTest, WriteAllEmpty) {
    manager_->open();
    
    std::vector<Employee> empty;
    EXPECT_TRUE(manager_->write_all(empty));
    
    auto employees = manager_->read_all();
    EXPECT_TRUE(employees.empty());
}

TEST_F(FileManagerTest, WriteAllWithData) {
    manager_->open();
    
    std::vector<Employee> expected = {
        Employee(1, "Alice", 38.0),
        Employee(2, "Bob", 42.5),
        Employee(3, "Charlie", 36.0)
    };
    
    EXPECT_TRUE(manager_->write_all(expected));
    
    auto actual = manager_->read_all();
    ASSERT_EQ(actual.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_TRUE(expected[i] == actual[i]);
    }
}

TEST_F(FileManagerTest, WriteAllOverwriteExisting) {
    std::vector<Employee> initial = {
        Employee(1, "Old1", 10.0),
        Employee(2, "Old2", 20.0)
    };
    CreateTestFile(initial);
    manager_->open();
    
    std::vector<Employee> new_data = {
        Employee(3, "New1", 30.0),
        Employee(4, "New2", 40.0),
        Employee(5, "New3", 50.0)
    };
    
    EXPECT_TRUE(manager_->write_all(new_data));
    
    auto actual = manager_->read_all();
    ASSERT_EQ(actual.size(), new_data.size());
    for (size_t i = 0; i < new_data.size(); ++i) {
        EXPECT_TRUE(new_data[i] == actual[i]);
    }
}

TEST_F(FileManagerTest, FindEmployee) {
    std::vector<Employee> employees = {
        Employee(1, "John", 40.0),
        Employee(2, "Jane", 35.5),
        Employee(3, "Bob", 42.0)
    };
    CreateTestFile(employees);
    manager_->open();
    
    auto loaded_employees = manager_->read_all();
    
    Employee* found = manager_->find_employee(loaded_employees, 2);
    EXPECT_NE(found, nullptr);
    EXPECT_TRUE(found->id == 2);
    EXPECT_STREQ(found->name, "Jane");
    
    Employee* not_found = manager_->find_employee(loaded_employees, 99);
    EXPECT_EQ(not_found, nullptr);
}

TEST_F(FileManagerTest, ConcurrentAccessProtection) {
    manager_->open();
    
    std::vector<Employee> initial_data = {
        Employee(1, "Test", 10.0)
    };
    EXPECT_TRUE(manager_->write_all(initial_data));
    
    std::vector<std::thread> threads;
    std::atomic<int> success_count{0};
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([this, i, &success_count]() {
            auto employees = this->manager_->read_all();
            if (!employees.empty()) {
                success_count++;
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    EXPECT_EQ(success_count, 10);
}

TEST_F(FileManagerTest, FileCorruptionRecovery) {
    std::ofstream bad_file(test_filename_, std::ios::binary);
    bad_file.write("corrupted data", 14);
    bad_file.close();
    
    EXPECT_TRUE(manager_->open());
    auto employees = manager_->read_all();
    EXPECT_TRUE(employees.empty());
    
    std::vector<Employee> new_data = {
        Employee(1, "Recovered", 45.0)
    };
    EXPECT_TRUE(manager_->write_all(new_data));
    
    employees = manager_->read_all();
    ASSERT_EQ(employees.size(), 1);
    EXPECT_TRUE(new_data[0] == employees[0]);
}

} 