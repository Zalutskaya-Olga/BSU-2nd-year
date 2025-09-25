#include <catch2/catch_test_macros.hpp>
#include "employee.h"

TEST_CASE("Employee creation and validation", "[employee]") {
    // Arrange
    int validId = 123;
    std::string validName = "John";
    double validHours = 40.5;
    
    // Act
    Employee emp(validId, validName, validHours);
    
    // Assert
    REQUIRE(emp.isValid());
    REQUIRE(emp.num == validId);
    REQUIRE(std::string(emp.name) == validName);
    REQUIRE(emp.hours == validHours);
}

TEST_CASE("Employee invalid data", "[employee]") {
    SECTION("Negative ID") {
        Employee emp(-1, "John", 40.0);
        REQUIRE_FALSE(emp.isValid());
    }
    
    SECTION("Negative hours") {
        Employee emp(123, "John", -5.0);
        REQUIRE_FALSE(emp.isValid());
    }
}