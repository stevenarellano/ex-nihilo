#include "circular_linked_list.h"

#include <gtest/gtest.h>

#include <iostream>

TEST(LinkedListNodeTest, DataAccessors) {
  LinkedListNode node;
  node.SetData(10);
  EXPECT_EQ(node.GetData(), 10);
}

TEST(LinkedListNodeTest, SetNextAndGetNext) {
  LinkedListNode node1, node2;
  node1.SetNext(&node2);
  EXPECT_EQ(node1.GetNext(), &node2);
}

TEST(LinkedListNodeTest, SetPrevAndGetPrev) {
  LinkedListNode node1, node2;
  node1.SetPrev(&node2);
  EXPECT_EQ(node1.GetPrev(), &node2);
}

TEST(LinkedListNodeTest, PrintNode) {
  LinkedListNode node;
  node.SetData(20);

  testing::internal::CaptureStdout();
  node.print();
  std::string output = testing::internal::GetCapturedStdout();
  EXPECT_EQ(output, "[CIRCULAR LINKED LIST NODE: 20]\n");
}

class CircularLinkedListTest : public ::testing::Test {
 protected:
  void SetUp() override { list = new CircularLinkedList(); }

  void TearDown() override { delete list; }

  CircularLinkedList* list;
};

TEST_F(CircularLinkedListTest, EmptyListInitialization) {
  EXPECT_EQ(list->GetHead()->GetNext(), list->GetHead());
  EXPECT_EQ(list->GetHead()->GetPrev(), list->GetHead());
}

TEST_F(CircularLinkedListTest, PushFrontSingleNode) {
  LinkedListNode* node = new LinkedListNode();
  node->SetData(1);
  list->push_front(node);

  EXPECT_EQ(list->GetHead()->GetNext(), node);
  EXPECT_EQ(list->GetHead()->GetPrev(), node);
  EXPECT_EQ(node->GetNext(), list->GetHead());
  EXPECT_EQ(node->GetPrev(), list->GetHead());
}

TEST_F(CircularLinkedListTest, PushBackSingleNode) {
  LinkedListNode* node = new LinkedListNode();
  node->SetData(2);
  list->push_back(node);

  EXPECT_EQ(list->GetHead()->GetNext(), node);
  EXPECT_EQ(list->GetHead()->GetPrev(), node);
  EXPECT_EQ(node->GetNext(), list->GetHead());
  EXPECT_EQ(node->GetPrev(), list->GetHead());
}

TEST_F(CircularLinkedListTest, PushFrontMultipleNodes) {
  LinkedListNode* node1 = new LinkedListNode();
  LinkedListNode* node2 = new LinkedListNode();
  node1->SetData(1);
  node2->SetData(2);

  list->push_front(node1);
  list->push_front(node2);

  EXPECT_EQ(list->GetHead()->GetNext(), node2);
  EXPECT_EQ(node2->GetNext(), node1);
  EXPECT_EQ(node1->GetNext(), list->GetHead());

  EXPECT_EQ(list->GetHead()->GetPrev(), node1);
  EXPECT_EQ(node1->GetPrev(), node2);
  EXPECT_EQ(node2->GetPrev(), list->GetHead());
}

TEST_F(CircularLinkedListTest, PushBackMultipleNodes) {
  LinkedListNode* node1 = new LinkedListNode();
  LinkedListNode* node2 = new LinkedListNode();
  node1->SetData(1);
  node2->SetData(2);

  list->push_back(node1);
  list->push_back(node2);

  EXPECT_EQ(list->GetHead()->GetNext(), node1);
  EXPECT_EQ(node1->GetNext(), node2);
  EXPECT_EQ(node2->GetNext(), list->GetHead());

  EXPECT_EQ(list->GetHead()->GetPrev(), node2);
  EXPECT_EQ(node2->GetPrev(), node1);
  EXPECT_EQ(node1->GetPrev(), list->GetHead());
}

TEST_F(CircularLinkedListTest, NodeRemoval) {
  LinkedListNode* node1 = new LinkedListNode();
  LinkedListNode* node2 = new LinkedListNode();
  LinkedListNode* node3 = new LinkedListNode();
  node1->SetData(1);
  node2->SetData(2);
  node3->SetData(3);

  list->push_back(node1);
  list->push_back(node2);
  list->push_back(node3);

  // Removing the middle node (node2)
  node2->remove();

  EXPECT_EQ(node1->GetNext(), node3);
  EXPECT_EQ(node3->GetPrev(), node1);
  EXPECT_EQ(list->GetHead()->GetNext(), node1);
  EXPECT_EQ(list->GetHead()->GetPrev(), node3);
}

TEST_F(CircularLinkedListTest, FullListPrint) {
  LinkedListNode* node1 = new LinkedListNode();
  LinkedListNode* node2 = new LinkedListNode();
  node1->SetData(10);
  node2->SetData(20);

  list->push_back(node1);
  list->push_back(node2);

  testing::internal::CaptureStdout();
  list->print();
  std::string output = testing::internal::GetCapturedStdout();

  std::string expectedOutput =
      "[CIRCULAR LINKED LIST]\n"
      "[CIRCULAR LINKED LIST NODE: 10]\n"
      "[CIRCULAR LINKED LIST NODE: 20]\n";
  EXPECT_EQ(output, expectedOutput);
}
