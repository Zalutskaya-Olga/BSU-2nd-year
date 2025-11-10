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
./thread_lab_test
