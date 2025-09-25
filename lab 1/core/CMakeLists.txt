# Core library
project(employee_core)

# Исходные файлы
set(SOURCES
    employee.cpp
)

# Заголовочные файлы
set(HEADERS
    employee.h
    file_utils.h
)

# Создаем библиотеку
add_library(employee_core STATIC ${SOURCES} ${HEADERS})

# Публичные заголовки
target_include_directories(employee_core
    PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# Свойства цели
set_target_properties(employee_core PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

# Компиляционные флаги
if(MSVC)
    target_compile_options(employee_core PRIVATE /W4 /WX)
else()
    target_compile_options(employee_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Установка
install(TARGETS employee_core
    EXPORT EmployeeManagementSystemTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

install(FILES ${HEADERS} DESTINATION include/core)