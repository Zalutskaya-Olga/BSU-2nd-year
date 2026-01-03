#include "employee_types.h"
#include <gtest/gtest.h>

namespace EmployeeSystem {

TEST(EmployeeTypesTest, EmployeeDefaultConstructor) {
    Employee emp;
    
    EXPECT_EQ(emp.id, 0);
    EXPECT_STREQ(emp.name, "");
    EXPECT_DOUBLE_EQ(emp.hours, 0.0);
}

TEST(EmployeeTypesTest, EmployeeParameterizedConstructor) {
    Employee emp(123, "John Doe", 40.5);
    
    EXPECT_EQ(emp.id, 123);
    EXPECT_STREQ(emp.name, "John Doe");
    EXPECT_DOUBLE_EQ(emp.hours, 40.5);
}

TEST(EmployeeTypesTest, EmployeeEqualityOperator) {
    Employee emp1(123, "John Doe", 40.5);
    Employee emp2(123, "John Doe", 40.5);
    Employee emp3(124, "Jane Doe", 35.0);
    
    EXPECT_TRUE(emp1 == emp2);
    EXPECT_FALSE(emp1 == emp3);
}

TEST(EmployeeTypesTest, EmployeeNameTruncation) {
    std::string long_name = "Very Long Name That Exceeds Limit";
    Employee emp(123, long_name, 40.5);
    
    EXPECT_EQ(std::string(emp.name).length(), 9);
    EXPECT_EQ(std::string(emp.name), "Very Long");
}

TEST(EmployeeTypesTest, RequestDefaultConstructor) {
    Request req;
    
    EXPECT_EQ(req.client_id, 0);
    EXPECT_EQ(req.employee_id, 0);
    EXPECT_EQ(req.operation, OperationType::READ);
    EXPECT_EQ(req.timestamp, 0);
}

TEST(EmployeeTypesTest, ResponseDefaultConstructor) {
    Response resp;
    
    EXPECT_EQ(resp.employee_id, 0);
    EXPECT_EQ(resp.status, ResponseStatus::ERROR);
    EXPECT_EQ(resp.timestamp, 0);
}

TEST(EmployeeTypesTest, OperationTypeValues) {
    EXPECT_EQ(static_cast<char>(OperationType::READ), 'R');
    EXPECT_EQ(static_cast<char>(OperationType::WRITE), 'W');
    EXPECT_EQ(static_cast<char>(OperationType::UNLOCK), 'U');
    EXPECT_EQ(static_cast<char>(OperationType::EXIT), 'X');
}

TEST(EmployeeTypesTest, ResponseStatusValues) {
    EXPECT_EQ(static_cast<char>(ResponseStatus::SUCCESS), 'S');
    EXPECT_EQ(static_cast<char>(ResponseStatus::ERROR), 'E');
    EXPECT_EQ(static_cast<char>(ResponseStatus::LOCKED), 'L');
    EXPECT_EQ(static_cast<char>(ResponseStatus::NOT_FOUND), 'N');
}

TEST(EmployeeTypesTest, StructSizes) {
    EXPECT_EQ(sizeof(Employee), sizeof(int32_t) + 10 + sizeof(double));
    EXPECT_EQ(sizeof(Request), sizeof(int32_t) * 2 + sizeof(OperationType) + sizeof(Employee) + sizeof(uint64_t));
    EXPECT_EQ(sizeof(Response), sizeof(int32_t) + sizeof(ResponseStatus) + sizeof(Employee) + sizeof(uint64_t));
}

TEST(EmployeeTypesTest, PackedAlignment) {
    Employee emp;
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&emp.id), reinterpret_cast<uintptr_t>(&emp));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&emp.name), reinterpret_cast<uintptr_t>(&emp.id) + sizeof(int32_t));
    EXPECT_EQ(reinterpret_cast<uintptr_t>(&emp.hours), reinterpret_cast<uintptr_t>(&emp.name) + 10);
}

} 