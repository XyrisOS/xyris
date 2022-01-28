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
        : m_Next(nullptr)
        , m_Prev(nullptr)
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
        : m_Next(n)
        , m_Prev(p)
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
        return m_Next;
    }

    /**
     * @brief Get the previous node in the linked list
     *
     * @return Node* Pointer to the previous node
     */
    Node* Previous()
    {
        return m_Prev;
    }

    /**
     * @brief Set the node's next pointer
     *
     * @param n Pointer to next node
     */
    void SetNext(Node* n)
    {
        m_Next = n;
    }

    /**
     * @brief Set the node's previous pointer
     *
     * @param n Pointer to the previous node
     */
    void SetPrevious(Node* n)
    {
        m_Prev = n;
    }

private:
    Node* m_Next;
    Node* m_Prev;
};

class LinkedListUnmanaged {
public:
    LinkedListUnmanaged()
        : head(nullptr)
        , tail(nullptr)
        , count(0)
    {
        // Default constructor
    }

    ~LinkedListUnmanaged()
    {
        // Unmanaged linked list does not destroy any nodes
    }

    void InsertFront(Node* node)
    {
        if (head) {
            InsertBefore(node, head);
        } else {
            head = node;
            tail = head;
            ++count;
        }
    }

    void InsertBack(Node* node)
    {
        if (tail) {
            InsertAfter(node, tail);
        } else {
            tail = node;
            head = tail;
            ++count;
        }
    }

    /**
     * @brief Insert a node before another.
     *
     * @param insert Node to be inserted.
     * @param node Node to be inserted before.
     */
    void InsertBefore(Node* insert, Node* node)
    {
        if (!insert)
            return;

        if (node->Previous()) {
            node->Previous()->SetNext(insert);
        } else {
            head = insert;
        }

        insert->SetPrevious(node->Previous());
        insert->SetNext(node);
        node->SetPrevious(insert);
        ++count;
    }

    void InsertAfter(Node* insert, Node* node)
    {
        if (!insert)
            return;

        if (node->Next()) {
            node->Next()->SetPrevious(insert);
        } else {
            tail = insert;
        }

        insert->SetNext(node->Next());
        insert->SetPrevious(node);
        node->SetNext(insert);
        ++count;
    }

    void Remove(Node* del)
    {
        if (!del)
            return;

        if (del == head)
            head = del->Next();
        if (del == tail)
            tail = del->Previous();
        if (del->Previous())
            del->Previous()->SetNext(del->Next());
        if (del->Next())
            del->Next()->SetPrevious(del->Previous());
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
