#ifndef KOLOK2_H
#define KOLOK2_H

#include <vector>
#include <stdexcept>
#include <string>


class ArrayUtils {
public:
  
    static std::vector<int> removeDuplicates(const std::vector<int>& input);
    static bool validateArray(const std::vector<int>& input);
    static std::string getVersion();

private:
    ArrayUtils();
    ArrayUtils(const ArrayUtils&);
    ArrayUtils& operator=(const ArrayUtils&);
    
    static const char* const LIBRARY_VERSION;
};

#endif 
