mkdir build
cd build

cmake ..

make

./thread_lab




cd build
rm -f CMakeCache.txt
rm -rf CMakeFiles/
cmake ..
make


# Запуск всех тестов
./thread_lab_test

# Запуск с детальным выводом
./thread_lab_test --gtest_verbose=1

# Запуск конкретного теста
./thread_lab_test --gtest_filter="ThreadManagerTest.FindMinMax"
