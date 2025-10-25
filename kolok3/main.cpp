#include "kolok3.h"
#include <iostream>
#include <stdexcept>

void demonstrateLinkedList() {
    std::cout << "Linked List Demonstration " << std::endl;
    
    LinkedList list;
    
    std::cout << "1. Creating list with values 1, 2, 3, 4, 5" << std::endl;
    list.append(1);
    list.append(2);
    list.append(3);
    list.append(4);
    list.append(5);
    
    std::cout << "Original list: ";
    list.print();
    
    std::cout << "2. Reversing list recursively" << std::endl;
    list.reverseRecursive();
    std::cout << "Reversed list: ";
    list.print();
    
    std::cout << "3. Reversing again (should return to original)" << std::endl;
    list.reverseRecursive();
    std::cout << "Double reversed: ";
    list.print();
    
    std::cout << "4. Testing edge cases" << std::endl;
    LinkedList emptyList;
    std::cout << "Empty list: ";
    emptyList.print();
    emptyList.reverseRecursive();
    std::cout << "Empty list after reverse: ";
    emptyList.print();
    
    LinkedList singleList;
    singleList.append(42);
    std::cout << "Single element list: ";
    singleList.print();
    singleList.reverseRecursive();
    std::cout << "Single element after reverse: ";
    singleList.print();
}

int main() {
    try {
        std::cout << "Kolok3: Linked List Reverse Recursive (C++98)" << std::endl;
        
        demonstrateLinkedList();
        
        std::cout << std::endl << "Demo completed successfully!" << std::endl;
        std::cout << "Run './kolok3_tests' for unit tests" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}