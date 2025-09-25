# CMake generated Testfile for 
# Source directory: /Users/olga_zalutskaya/Documents/oc/lab1/tests
# Build directory: /Users/olga_zalutskaya/Documents/oc/lab1/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(employee_unit_tests "/Users/olga_zalutskaya/Documents/oc/lab1/tests/unit_tests")
set_tests_properties(employee_unit_tests PROPERTIES  LABELS "unit" _BACKTRACE_TRIPLES "/Users/olga_zalutskaya/Documents/oc/lab1/tests/CMakeLists.txt;53;add_test;/Users/olga_zalutskaya/Documents/oc/lab1/tests/CMakeLists.txt;0;")
add_test(employee_e2e_tests "/Users/olga_zalutskaya/Documents/oc/lab1/tests/e2e_tests")
set_tests_properties(employee_e2e_tests PROPERTIES  LABELS "e2e" _BACKTRACE_TRIPLES "/Users/olga_zalutskaya/Documents/oc/lab1/tests/CMakeLists.txt;54;add_test;/Users/olga_zalutskaya/Documents/oc/lab1/tests/CMakeLists.txt;0;")
