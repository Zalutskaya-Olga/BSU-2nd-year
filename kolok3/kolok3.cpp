#include "kolok3.h"
#include <iostream>

LinkedList::LinkedList() : head(NULL) {
}

LinkedList::~LinkedList() {
    clear();
}

void LinkedList::append(int data) {
    ListNode* newNode = new ListNode(data);
    
    if (isEmpty()) {
        head = newNode;
        return;
    }
    
    ListNode* current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = newNode;
}

void LinkedList::reverseRecursive() {
    if (isEmpty() || head->next == NULL) {
        return;
    }
    
    head = reverseRecursiveHelper(head);
}

ListNode* LinkedList::reverseRecursiveHelper(ListNode* current) {
    if (current == NULL || current->next == NULL) {
        return current;
    }
    
    ListNode* newHead = reverseRecursiveHelper(current->next);
    current->next->next = current;
    current->next = NULL;
    
    return newHead;
}

void LinkedList::print() const {
    if (isEmpty()) {
        std::cout << "List is empty" << std::endl;
        return;
    }
    
    ListNode* current = head;
    while (current != NULL) {
        std::cout << current->data;
        if (current->next != NULL) {
            std::cout << " -> ";
        }
        current = current->next;
    }
    std::cout << std::endl;
}

bool LinkedList::isEmpty() const {
    return head == NULL;
}

void LinkedList::clear() {
    ListNode* current = head;
    while (current != NULL) {
        ListNode* next = current->next;
        delete current;
        current = next;
    }
    head = NULL;
}