#include <gtest/gtest.h>
#include <iostream>
#include "employee_types.h"
#include "file_manager.h"
#include "lock_manager.h"
#include "logger.h"

namespace EmployeeSystem {

class DemoTest : public ::testing::Test {
protected:
    void SetUp() override {
        std::cout << "\n==========================================\n";
        std::cout << "Демонстрационный тест\n";
        std::cout << "==========================================\n";
    }
};

TEST_F(DemoTest, CompleteSystemDemo) {
    std::cout << "\n1. Создание и работа с Employee:\n";
    Employee emp(1001, "Иван Петров", 176.5);
    std::cout << "   Создан сотрудник: ID=" << emp.id 
              << ", Имя='" << emp.name 
              << "', Часы=" << emp.hours << std::endl;
    
    std::cout << "\n2. Работа с FileManager:\n";
    FileManager file_manager("demo_test.dat");
    file_manager.open();
    
    std::vector<Employee> employees = {
        Employee(1001, "Анна", 160.0),
        Employee(1002, "Борис", 180.5),
        Employee(1003, "Светлана", 155.0)
    };
    
    file_manager.write_all(employees);
    std::cout << "   Записано " << employees.size() << " сотрудников\n";
    
    auto read_employees = file_manager.read_all();
    std::cout << "   Прочитано " << read_employees.size() << " сотрудников\n";
    
    std::cout << "\n3. Работа с LockManager:\n";
    LockManager lock_manager;
    
    bool can_read = lock_manager.acquire_read_lock(1001, 1);
    std::cout << "   Клиент 1 может читать сотрудника 1001: " 
              << (can_read ? "ДА" : "НЕТ") << std::endl;
    
    bool can_write = lock_manager.acquire_write_lock(1001, 2);
    std::cout << "   Клиент 2 может писать сотрудника 1001: " 
              << (can_write ? "ДА" : "НЕТ") << std::endl;
    
    lock_manager.release_read_lock(1001, 1);
    std::cout << "   Клиент 1 освободил блокировку\n";
    
    can_write = lock_manager.acquire_write_lock(1001, 2);
    std::cout << "   Теперь клиент 2 может писать сотрудника 1001: " 
              << (can_write ? "ДА" : "НЕТ") << std::endl;
    
    std::cout << "\n4. Логирование:\n";
    Logger::info("Демонстрационный тест завершен успешно");
    
    file_manager.close();
    std::filesystem::remove("demo_test.dat");
    
    EXPECT_TRUE(can_read);
    EXPECT_FALSE(can_write); 
}

TEST_F(DemoTest, RequestResponseDemo) {
    std::cout << "\nДемонстрация запросов и ответов:\n";
    
    Request req;
    req.client_id = 101;
    req.employee_id = 2001;
    req.operation = OperationType::READ;
    req.timestamp = 1234567890;
    
    std::cout << "   Создан запрос: клиент=" << req.client_id
              << ", сотрудник=" << req.employee_id
              << ", операция=READ" << std::endl;
    
    Response resp;
    resp.employee_id = req.employee_id;
    resp.status = ResponseStatus::SUCCESS;
    resp.employee = Employee(2001, "Демо", 100.0);
    resp.timestamp = req.timestamp + 1;
    
    std::cout << "   Создан ответ: статус=SUCCESS"
              << ", имя сотрудника=" << resp.employee.name << std::endl;
    
    EXPECT_EQ(req.employee_id, resp.employee_id);
    EXPECT_EQ(resp.status, ResponseStatus::SUCCESS);
}

} 

int main(int argc, char** argv) {
    std::cout << "==========================================\n";
    std::cout << "ДЕМОНСТРАЦИЯ РАБОТЫ EMPLOYEE SYSTEM\n";
    std::cout << "==========================================\n";
    
    ::testing::InitGoogleTest(&argc, argv);
    
    ::testing::GTEST_FLAG(filter) = "*DemoTest*";
    
    int result = RUN_ALL_TESTS();
    
    std::cout << "\n==========================================\n";
    std::cout << "Демонстрация завершена\n";
    std::cout << "==========================================\n";
    
    return result;
}