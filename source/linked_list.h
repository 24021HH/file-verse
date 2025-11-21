#pragma once
#include <cstddef>
using namespace std;

template <typename T>
class LinkedList {
private:
struct Node {
T data;
Node* next;
Node(const T& val): data(val), next(nullptr) {}
};


Node* head;
Node* tail;


public:
class Iterator {
Node* current;
public:
Iterator(Node* node) : current(node) {}
bool operator!=(const Iterator& other) const { return current != other.current; }
Iterator& operator++() { if (current) current = current->next; return *this; }
T& operator*() { return current->data; }
};


LinkedList() : head(nullptr), tail(nullptr) {}


~LinkedList() { clear(); }


void push_back(const T& val) {
Node* node = new Node(val);
if (!head) head = tail = node;
else { tail->next = node; tail = node; }
}


template<typename Predicate>
void remove_if(Predicate pred) {
Node* prev = nullptr;
Node* curr = head;
while (curr) {
if (pred(curr->data)) {
Node* toDelete = curr;
if (prev) prev->next = curr->next;
else head = curr->next;
curr = curr->next;
delete toDelete;
} else {
prev = curr;
curr = curr->next;
}
}
tail = head;
if (!tail) return;
while (tail->next) tail = tail->next;
}


void clear() {
Node* cur = head;
while (cur) {
Node* next = cur->next;
delete cur;
cur = next;
}
head = tail = nullptr;
}


Iterator begin() { return Iterator(head); }
Iterator end() { return Iterator(nullptr); }
};
