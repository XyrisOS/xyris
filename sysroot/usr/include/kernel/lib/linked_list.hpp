/**
 * @file linked_list.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Standard linked list library
 * @version 0.3
 * @date 2020-06-17
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 * Thanks for mrvn on the OSDev Wiki for the implementation:
 * Linked List Bucket Heap 2013 Goswin von Brederlow <goswin-v-b@web.de>
 */
#pragma once

#include <stdint.h>
#include <stddef.h>

#define DLIST_REMOVE_FROM(h, d, l)                      \
    {                                                   \
        __typeof__(**h) **h_ = h, *d_ = d;              \
        DList *head = &(*h_)->l;                        \
        dlist_remove_from(&head, &d_->l);               \
        if (head == NULL) {                             \
            *h_ = NULL;                                 \
        } else {                                        \
            *h_ = CONTAINER(__typeof__(**h), l, head);  \
        }                                               \
    }

#define DLIST_PUSH(h, v, l)                             \
    {                                                   \
        __typeof__(*v) **h_ = h, *v_ = v;               \
        DList *head = &(*h_)->l;                        \
        if (*h_ == NULL) head = NULL;                   \
        dlist_push(&head, &v_->l);                      \
        *h_ = CONTAINER(__typeof__(*v), l, head);       \
    }

#define DLIST_POP(h, l)                                 \
    ({                                                  \
    __typeof__(**h) **h_ = h;                           \
    DList *head = &(*h_)->l;                            \
    DList *res = dlist_pop(&head);                      \
    if (head == NULL) {                                 \
        *h_ = NULL;                                     \
    } else {                                            \
        *h_ = CONTAINER(__typeof__(**h), l, head);      \
    }                                                   \
    CONTAINER(__typeof__(**h), l, res);                 \
    })

#define DLIST_ITERATOR_BEGIN(h, l, it)                  \
    {                                                   \
        __typeof__(*h) *h_ = h;                         \
        DList *last_##it = h_->l.prev, *iter_##it = &h_->l, *next_##it; \
    do {                                                \
        if (iter_##it == last_##it) {                   \
            next_##it = NULL;                           \
        } else {                                        \
            next_##it = iter_##it->next;                \
        }                                               \
        __typeof__(*h)* it = CONTAINER(__typeof__(*h), l, iter_##it);

#define DLIST_ITERATOR_END(it)                          \
    } while((iter_##it = next_##it));                   \
    }

#define DLIST_ITERATOR_REMOVE_FROM(h, it, l) DLIST_REMOVE_FROM(h, iter_##it, l)

typedef struct DList {
    DList *next;
    DList *prev;
} DList;

/**
 * @brief Initializes a linked link.
 * 
 * @param dlist Doubly linked list
 */
void dlist_init(DList *dlist);
/**
 * @brief Inserts D2 after D1 in the linked list.
 * 
 * @param d1 Parent node
 * @param d2 Child node
 */
void dlist_insert_after(DList *d1, DList *d2);
/**
 * @brief Inserts D2 before D1.
 * 
 * @param d1 Child node
 * @param d2 Parent node
 */
void dlist_insert_before(DList *d1, DList *d2);
/**
 * @brief Removes a node from the linked list.
 * 
 * @param d Node to be removed
 */
void dlist_remove(DList *d);
/**
 * @brief Pushes a node onto the end of the linked list.
 * 
 * @param d1p Linked list
 * @param d2 Node to be pushed
 */
void dlist_push(DList **d1p, DList *d2);
/**
 * @brief Pops the last node off of the linked list.
 * 
 * @param dp Linked list
 * @return DList* Popped node
 */
DList* dlist_pop(DList **dp);
/**
 * @brief Removes a given node from a linked list.
 * 
 * @param d1p Linked list
 * @param d2 Node to be removed
 */
void dlist_remove_from(DList **d1p, DList *d2);
