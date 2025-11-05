#ifndef ARRAY_MANAGER_H
#define ARRAY_MANAGER_H

class ArrayManager {
public:
    explicit ArrayManager(int size);
    ~ArrayManager();
    
    int getSize() const;
    int getElement(int index) const;
    void setElement(int index, int value);
    void initializeWithZeros();
    void printArray() const;
    
private:
    int* m_array;
    int m_size;
    
    // Prevent copying
    ArrayManager(const ArrayManager&);
    ArrayManager& operator=(const ArrayManager&);
};

#endif