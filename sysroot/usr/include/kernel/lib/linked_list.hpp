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

#ifndef PANIX_LIB_LINKED_LIST
#define PANIX_LIB_LINKED_LIST

#include <sys/types.hpp>

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

#endif /* PANIX_LIB_LINKED_LIST */
