#include <gtest/gtest.h>
#include "common.h"
#include <string>
#include <cstring>

TEST(CommonTest, MsgStructureTest) {
    Msg msg;
    
    EXPECT_EQ(sizeof(msg.text), MAX_LEN + 1);
    
    const char* test_string = "Hello, World!";
    strcpy(msg.text, test_string);
    EXPECT_STREQ(msg.text, test_string);
    
    msg.is_valid = true;
    EXPECT_TRUE(msg.is_valid);
    msg.is_valid = false;
    EXPECT_FALSE(msg.is_valid);
}

TEST(CommonTest, SharedDataStructureTest) {
    SharedData data;
    
    EXPECT_EQ(sizeof(data.messages), sizeof(Msg) * MAX_FILE);
    
    data.sender_count = 3;
    data.ready_senders = 1;
    EXPECT_EQ(data.sender_count, 3);
    EXPECT_EQ(data.ready_senders, 1);
    
    const char* test_msg = "Test message";
    strcpy(data.messages[0].text, test_msg);
    data.messages[0].is_valid = true;
    
    EXPECT_STREQ(data.messages[0].text, test_msg);
    EXPECT_TRUE(data.messages[0].is_valid);
}

TEST(CommonTest, ConstantsTest) {
    EXPECT_EQ(MAX_LEN, 20);
    EXPECT_EQ(MAX_FILE, 100);
    
    char buffer[MAX_LEN + 1];
    std::string test_string(MAX_LEN, 'A');
    strcpy(buffer, test_string.c_str());
    EXPECT_EQ(strlen(buffer), MAX_LEN);
}

TEST(CommonTest, MessageLengthBoundaryTest) {
    Msg msg;
    
    std::string max_length_msg(MAX_LEN, 'X');
    strcpy(msg.text, max_length_msg.c_str());
    EXPECT_EQ(strlen(msg.text), MAX_LEN);
    
    std::string short_msg = "Hi";
    strcpy(msg.text, short_msg.c_str());
    EXPECT_STREQ(msg.text, short_msg.c_str());
}

TEST(CommonTest, MessageBufferOverflowTest) {
    Msg msg;
    
    std::string full_message(MAX_LEN, 'Z');
    strcpy(msg.text, full_message.c_str());
    
    EXPECT_EQ(strlen(msg.text), MAX_LEN);
    EXPECT_EQ(msg.text[MAX_LEN], '\0'); 
}

TEST(CommonTest, StructureSizeTest) {
    EXPECT_EQ(sizeof(Msg), sizeof(char) * (MAX_LEN + 1) + sizeof(bool));
    EXPECT_GE(sizeof(SharedData), sizeof(Msg) * MAX_FILE + 4 * sizeof(int) + 2 * sizeof(bool));
}

TEST(CommonTest, IndexOperationsTest) {
    SharedData data;
    data.max_messages = 10;
    data.read_index = 0;
    data.write_index = 0;
    
    data.write_index = (data.write_index + 1) % data.max_messages;
    EXPECT_EQ(data.write_index, 1);
    
    data.write_index = (data.write_index + 1) % data.max_messages;
    EXPECT_EQ(data.write_index, 2);
    
    data.write_index = (data.write_index + 8) % data.max_messages;
    EXPECT_EQ(data.write_index, 0);
}

TEST(CommonTest, CircularBufferTest) {
    SharedData data;
    data.max_messages = 5;
    data.read_index = 0;
    data.write_index = 0;
    data.count = 0;
    
    EXPECT_EQ(data.count, 0);
    
    strcpy(data.messages[data.write_index].text, "Msg1");
    data.messages[data.write_index].is_valid = true;
    data.write_index = (data.write_index + 1) % data.max_messages;
    data.count++;
    
    EXPECT_EQ(data.count, 1);
    EXPECT_STREQ(data.messages[0].text, "Msg1");
    
    strcpy(data.messages[data.write_index].text, "Msg2");
    data.messages[data.write_index].is_valid = true;
    data.write_index = (data.write_index + 1) % data.max_messages;
    data.count++;
    
    EXPECT_EQ(data.count, 2);
    EXPECT_STREQ(data.messages[1].text, "Msg2");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}