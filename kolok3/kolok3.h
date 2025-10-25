#ifndef KOLOK3_H
#define KOLOK3_H

#include <cstddef> 

class ListNode;

class LinkedList {
public:
    LinkedList();
    ~LinkedList();
    
    void append(int data);
    void reverseRecursive();
    void print() const;
    bool isEmpty() const;
    void clear();

private:
    ListNode* head;
    ListNode* reverseRecursiveHelper(ListNode* current);
};

class ListNode {
public:
    explicit ListNode(int data) : data(data), next(NULL) {}
    friend class LinkedList;

private:
    int data;
    ListNode* next;
};

#endif