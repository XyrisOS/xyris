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
    Node()
        : m_Next(nullptr)
        , m_Prev(nullptr)
    {
        // Default constructor
    }

    Node(Node* n, Node* p)
        : m_Next(n)
        , m_Prev(p)
    {
        // Complete constructor
    }

    Node* Next() { return m_Next; }
    Node* Previous() { return m_Prev; }
    void SetNext(Node* n) { m_Next = n; }
    void SetPrevious(Node* n) { m_Prev = n; }

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

    ~LinkedListUnmanaged() = default;

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

    Node* Head()
    {
        return head;
    }

    Node* Tail()
    {
        return tail;
    }

    size_t Count()
    {
        return count;
    }

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
