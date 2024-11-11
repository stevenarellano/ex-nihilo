#ifndef CIRCULAR_LINKED_LIST_H
#define CIRCULAR_LINKED_LIST_H

#include <iostream>

class LinkedListNode {
 public:
  void print() {
    std::cout << "[" << "CIRCULAR LINKED LIST NODE: " << data_ << "]"
              << std::endl;
  }
  void remove() {
    if (prev_) {
      prev_->next_ = next_;
    }
    if (next_) {
      next_->prev_ = prev_;
    }
    delete this;
  }

  int GetData() const { return data_; }
  void SetData(int data) { data_ = data; }
  LinkedListNode *GetNext() const { return next_; }
  void SetNext(LinkedListNode *next) { next_ = next; }
  LinkedListNode *GetPrev() const { return prev_; }
  void SetPrev(LinkedListNode *prev) { prev_ = prev; }

 private:
  int data_;
  LinkedListNode *next_;
  LinkedListNode *prev_;
};

class CircularLinkedList {
 public:
  CircularLinkedList() {
    head_ = new LinkedListNode();
    head_->SetNext(head_);
    head_->SetPrev(head_);
  }
  ~CircularLinkedList() {
    LinkedListNode *current = head_->GetNext();
    while (current != head_) {
      LinkedListNode *next = current->GetNext();
      current->remove();
      current = next;
    }
    delete head_;
  }

  void print() const {
    std::cout << "[CIRCULAR LINKED LIST]" << std::endl;
    LinkedListNode *current = head_->GetNext();
    while (current != head_) {
      current->print();
      current = current->GetNext();
    }
  }

  void push_front(LinkedListNode *node) {
    node->SetNext(head_->GetNext());
    node->SetPrev(head_);
    head_->GetNext()->SetPrev(node);
    head_->SetNext(node);
  }
  void push_back(LinkedListNode *node) {
    node->SetNext(head_);
    node->SetPrev(head_->GetPrev());
    head_->GetPrev()->SetNext(node);
    head_->SetPrev(node);
  }

  LinkedListNode *GetHead() const { return head_; }

 private:
  LinkedListNode *head_;
};

#endif  // CIRCULAR_LINKED_LIST_H