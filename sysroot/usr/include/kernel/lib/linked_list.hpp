/**
 * @file linked_list.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Standard linked list library
 * @version 0.1
 * @date 2020-06-17
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 * Thanks for mrvn on the OSDev Wiki for the implementation:
 * Linked List Bucket Heap 2013 Goswin von Brederlow <goswin-v-b@web.de>
 */

#ifndef PANIX_LIB_LINKED_LIST
#define PANIX_LIB_LINKED_LIST

#include <sys/panix.hpp>

typedef struct DList {
    DList *next;
    DList *prev;
} DList;

static inline void dlist_init(DList *dlist);
static inline void dlist_insert_after(DList *d1, DList *d2);
static inline void dlist_insert_before(DList *d1, DList *d2);
static inline void dlist_remove(DList *d);
static inline void dlist_push(DList **d1p, DList *d2);
static inline DList* dlist_pop(DList **dp);
static inline void dlist_remove_from(DList **d1p, DList *d2);

#endif /* PANIX_LIB_LINKED_LIST */