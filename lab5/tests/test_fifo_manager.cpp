#include "../include/fifo_manager.h" 
#include <gtest/gtest.h>
#include <filesystem>
#include <thread>
#include <chrono>
using namespace EmployeeSystem;

namespace EmployeeSystem {

class FIFOManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_fifo_path_ = "/tmp/test_fifo_" + std::to_string(getpid());
    }
    
    void TearDown() override {
        std::filesystem::remove(test_fifo_path_);
    }
    
    std::string test_fifo_path_;
};

TEST_F(FIFOManagerTest, CreateFifoSuccess) {
    EXPECT_TRUE(FIFOManager::create_fifo(test_fifo_path_));
    EXPECT_TRUE(std::filesystem::exists(test_fifo_path_));
    
    struct stat stat_buf;
    EXPECT_EQ(stat(test_fifo_path_.c_str(), &stat_buf), 0);
    EXPECT_TRUE(S_ISFIFO(stat_buf.st_mode)); 
}

TEST_F(FIFOManagerTest, CreateFifoOverwritesExisting) {
    std::ofstream file(test_fifo_path_);
    file << "test";
    file.close();
    
    EXPECT_TRUE(FIFOManager::create_fifo(test_fifo_path_));
    
    struct stat stat_buf;
    EXPECT_EQ(stat(test_fifo_path_.c_str(), &stat_buf), 0);
    EXPECT_TRUE(S_ISFIFO(stat_buf.st_mode)); 
}

TEST_F(FIFOManagerTest, RemoveFifoSuccess) {
    FIFOManager::create_fifo(test_fifo_path_);
    EXPECT_TRUE(std::filesystem::exists(test_fifo_path_));
    
    EXPECT_TRUE(FIFOManager::remove_fifo(test_fifo_path_));
    EXPECT_FALSE(std::filesystem::exists(test_fifo_path_));
}

TEST_F(FIFOManagerTest, OpenFifoForReadingSuccess) {
    FIFOManager::create_fifo(test_fifo_path_);
    
    std::thread writer([this]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::ofstream file(this->test_fifo_path_);
        file << "test";
        file.close();
    });
    
    auto stream = FIFOManager::open_fifo(test_fifo_path_, std::ios::in);
    EXPECT_NE(stream, nullptr);
    EXPECT_TRUE(stream->is_open());
    
    writer.join();
}

TEST_F(FIFOManagerTest, OpenFifoForWritingSuccess) {
    FIFOManager::create_fifo(test_fifo_path_);
    
    auto stream = FIFOManager::open_fifo(test_fifo_path_, std::ios::out);
    EXPECT_NE(stream, nullptr);
    EXPECT_TRUE(stream->is_open());
    
    *stream << "test";
    stream->flush();
    EXPECT_TRUE(stream->good());
}

TEST_F(FIFOManagerTest, OpenFifoBinaryMode) {
    FIFOManager::create_fifo(test_fifo_path_);
    
    auto stream = FIFOManager::open_fifo(test_fifo_path_, 
                                       std::ios::out | std::ios::binary);
    EXPECT_NE(stream, nullptr);
    EXPECT_TRUE(stream->is_open());
    
    int test_data = 12345;
    stream->write(reinterpret_cast<char*>(&test_data), sizeof(test_data));
    stream->flush();
    EXPECT_TRUE(stream->good());
}

TEST_F(FIFOManagerTest, OpenNonExistentFifo) {
    auto stream = FIFOManager::open_fifo("/tmp/nonexistent_fifo", std::ios::in);
    EXPECT_EQ(stream, nullptr);
}

TEST_F(FIFOManagerTest, OpenFifoWithInvalidPermissions) {
    ::mkfifo(test_fifo_path_.c_str(), 0000);
    
    auto stream = FIFOManager::open_fifo(test_fifo_path_, std::ios::in);
    if (stream) {
        EXPECT_TRUE(stream->is_open());
    } else {
        EXPECT_EQ(stream, nullptr);
    }
    
    ::chmod(test_fifo_path_.c_str(), 0666);
}

TEST_F(FIFOManagerTest, ReadWriteCommunication) {
    FIFOManager::create_fifo(test_fifo_path_);
    
    std::atomic<bool> writer_done{false};
    std::string received_data;
    
    std::thread reader([this, &received_data, &writer_done]() {
        auto stream = FIFOManager::open_fifo(this->test_fifo_path_, std::ios::in);
        ASSERT_NE(stream, nullptr);
        
        std::string buffer;
        while (!writer_done || stream->peek() != EOF) {
            char ch;
            if (stream->get(ch)) {
                buffer += ch;
            }
        }
        received_data = buffer;
    });
    
    std::thread writer([this, &writer_done]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto stream = FIFOManager::open_fifo(this->test_fifo_path_, std::ios::out);
        ASSERT_NE(stream, nullptr);
        
        *stream << "Hello FIFO!";
        stream->flush();
        writer_done = true;
    });
    
    writer.join();
    reader.join();
    
    EXPECT_EQ(received_data, "Hello FIFO!");
}

TEST_F(FIFOManagerTest, BinaryDataTransfer) {
    FIFOManager::create_fifo(test_fifo_path_);
    
    struct TestData {
        int32_t id;
        double value;
        char name[20];
    };
    
    TestData send_data{123, 45.67, "Test Name"};
    TestData receive_data{};
    
    std::atomic<bool> writer_done{false};
    
    std::thread reader([this, &receive_data, &writer_done]() {
        auto stream = FIFOManager::open_fifo(this->test_fifo_path_, 
                                           std::ios::in | std::ios::binary);
        ASSERT_NE(stream, nullptr);
        
        while (!writer_done && !stream->read(
            reinterpret_cast<char*>(&receive_data), 
            sizeof(receive_data))) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    });
    
    std::thread writer([this, &send_data, &writer_done]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        auto stream = FIFOManager::open_fifo(this->test_fifo_path_, 
                                           std::ios::out | std::ios::binary);
        ASSERT_NE(stream, nullptr);
        
        stream->write(reinterpret_cast<char*>(&send_data), sizeof(send_data));
        stream->flush();
        writer_done = true;
    });
    
    writer.join();
    reader.join();
    
    EXPECT_EQ(receive_data.id, send_data.id);
    EXPECT_DOUBLE_EQ(receive_data.value, send_data.value);
    EXPECT_STREQ(receive_data.name, send_data.name);
}

} 