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

class LinkedListNode {
public:
    /**
     * @brief Construct a new Linked List Node object
     *
     */
    LinkedListNode()
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
    LinkedListNode(LinkedListNode* n, LinkedListNode* p)
        : next(n)
        , prev(p)
    {
        // Complete constructor
    }

    /**
     * @brief Get the next node in the linked list
     *
     * @return LinkedListNode* Pointer to next node
     */
    LinkedListNode* Next()
    {
        return next;
    }

    /**
     * @brief Get the previous node in the linked list
     *
     * @return LinkedListNode* Pointer to the previous node
     */
    LinkedListNode* Previous()
    {
        return prev;
    }

    /**
     * @brief Set the node's next pointer
     *
     * @param n Pointer to next node
     */
    void SetNext(LinkedListNode* n)
    {
        next = n;
    }

    /**
     * @brief Set the node's previous pointer
     *
     * @param n Pointer to the previous node
     */
    void SetPrevious(LinkedListNode* n)
    {
        prev = n;
    }

private:
    LinkedListNode* next;
    LinkedListNode* prev;
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
        LinkedListNode* back;
        while ((back = RemoveBack())) {
            delete back;
        }
    }

    void InsertFront(LinkedListNode* val)
    {
        if (head) {
            InsertBefore(head, val);
        } else {
            head = val;
            tail = head;
            ++count;
        }
    }

    void InsertBack(LinkedListNode* node)
    {
        if (tail) {
            InsertAfter(tail, node);
        } else {
            tail = node;
            head = tail;
            ++count;
        }
    }

    void InsertBefore(LinkedListNode* next, LinkedListNode* node)
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

    void InsertAfter(LinkedListNode* prev, LinkedListNode* node)
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

    void Remove(LinkedListNode* del)
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

    LinkedListNode* RemoveFront()
    {
        if (!head)
            return NULL;
        LinkedListNode* currHead = head;
        Remove(currHead);
        return currHead;
    }

    LinkedListNode* RemoveBack()
    {
        if (!tail)
            return NULL;
        LinkedListNode* currTail = tail;
        Remove(currTail);
        return currTail;
    }

    LinkedListNode* RemoveBefore(LinkedListNode* node)
    {
        if (!node)
            return NULL;
        LinkedListNode* before = node->Previous();
        Remove(before);
        return before;
    }

    LinkedListNode* RemoveAfter(LinkedListNode* node)
    {
        if (!node)
            return NULL;
        LinkedListNode* after = node->Next();
        Remove(after);
        return after;
    }

    /**
     * @brief Get pointer to the head node
     *
     * @return LinkedListNode* Pointer to head node
     */
    LinkedListNode* Head()
    {
        return head;
    }

    /**
     * @brief Get pointer to the tail node
     *
     * @return LinkedListNode* Pointer to tail node
     */
    LinkedListNode* Tail()
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
    LinkedListNode* head;
    LinkedListNode* tail;
    size_t count;
};

} // !namespace LinkedList
