/**
 * @file LinkedList.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Standard linked list library
 * @version 0.3
 * @date 2020-06-17
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace LinkedList {

class Node {
public:
    /**
     * @brief Construct a new Linked List Node object
     *
     */
    Node()
        : next(nullptr)
        , prev(nullptr)
    {
        // Default constructor
    }

    /**
     * @brief Construct a new Linked List Node object
     *
     * @param n Next node in the list
     * @param p Previous node in the list
     */
    Node(Node* n, Node* p)
        : next(n)
        , prev(p)
    {
        // Complete constructor
    }

    /**
     * @brief Get the next node in the linked list
     *
     * @return Node* Pointer to next node
     */
    Node* Next()
    {
        return next;
    }

    /**
     * @brief Get the previous node in the linked list
     *
     * @return Node* Pointer to the previous node
     */
    Node* Previous()
    {
        return prev;
    }

    /**
     * @brief Set the node's next pointer
     *
     * @param n Pointer to next node
     */
    void SetNext(Node* n)
    {
        next = n;
    }

    /**
     * @brief Set the node's previous pointer
     *
     * @param n Pointer to the previous node
     */
    void SetPrevious(Node* n)
    {
        prev = n;
    }

private:
    Node* next;
    Node* prev;
};

class LinkedList {
public:
    LinkedList()
        : head(nullptr)
        , tail(nullptr)
        , count(0)
    {
        // Default constructor
    }

    ~LinkedList()
    {
        Node* back;
        while ((back = RemoveBack())) {
            delete back;
        }
    }

    void InsertFront(Node* val)
    {
        if (head) {
            InsertBefore(head, val);
        } else {
            head = val;
            tail = head;
            ++count;
        }
    }

    void InsertBack(Node* node)
    {
        if (tail) {
            InsertAfter(tail, node);
        } else {
            tail = node;
            head = tail;
            ++count;
        }
    }

    void InsertBefore(Node* next, Node* node)
    {
        if (!next)
            return;
        node->SetPrevious(next->Previous());
        next->SetPrevious(node);
        node->SetNext(next);
        if (node->Previous()) {
            node->Previous()->SetNext(node);
        } else {
            head = node;
        }
        ++count;
    }

    void InsertAfter(Node* prev, Node* node)
    {
        if (!prev)
            return;
        node->SetNext(prev->Next());
        prev->SetNext(node);
        node->SetPrevious(prev);
        if (node->Next()) {
            node->Next()->SetPrevious(node);
        } else {
            tail = node;
        }
        ++count;
    }

    void Remove(Node* del)
    {
        if (del == head)
            head = del->Next();
        if (del == tail)
            tail = del->Previous();
        if (del->Next())
            del->Next()->SetPrevious(del->Previous());
        if (del->Previous())
            del->Previous()->SetNext(del->Next());
        count--;
    }

    Node* RemoveFront()
    {
        if (!head)
            return NULL;
        Node* currHead = head;
        Remove(currHead);
        return currHead;
    }

    Node* RemoveBack()
    {
        if (!tail)
            return NULL;
        Node* currTail = tail;
        Remove(currTail);
        return currTail;
    }

    Node* RemoveBefore(Node* node)
    {
        if (!node)
            return NULL;
        Node* before = node->Previous();
        Remove(before);
        return before;
    }

    Node* RemoveAfter(Node* node)
    {
        if (!node)
            return NULL;
        Node* after = node->Next();
        Remove(after);
        return after;
    }

    /**
     * @brief Get pointer to the head node
     *
     * @return Node* Pointer to head node
     */
    Node* Head()
    {
        return head;
    }

    /**
     * @brief Get pointer to the tail node
     *
     * @return Node* Pointer to tail node
     */
    Node* Tail()
    {
        return tail;
    }

    /**
     * @brief Get the number of items in the linked list
     *
     * @return size_t Number of items in the linked list
     */
    size_t Count()
    {
        return count;
    }

    /**
     * @brief Check if the linked list is empty
     *
     * @return true The list is empty
     * @return false The list is not empty
     */
    bool IsEmpty()
    {
        return Count() == 0;
    }

private:
    Node* head;
    Node* tail;
    size_t count;
};

} // !namespace LinkedList
