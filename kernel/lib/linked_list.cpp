/**
 * @file linked_list.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Standard linked list library
 * @version 0.3
 * @date 2020-06-18
 *
 * @copyright Copyright the Panix Contributors (c) 2020
 * Thanks for mrvn on the OSDev Wiki for the implementation:
 * Linked List Bucket Heap 2013 Goswin von Brederlow <goswin-v-b@web.de>
 *
 */

#include <lib/linked_list.hpp>
#include <lib/stdio.hpp>

// initialize a one element DList
void dlist_init(DList *dlist) {
    //kprintf("%s(%p)\n", __FUNCTION__, dlist);
    dlist->next = dlist;
    dlist->prev = dlist;
}

// insert d2 after d1
void dlist_insert_after(DList *d1, DList *d2) {
    //kprintf("%s(%p, %p)\n", __FUNCTION__, d1, d2);
    DList *n1 = d1->next;
    DList *e2 = d2->prev;

    d1->next = d2;
    d2->prev = d1;
    e2->next = n1;
    n1->prev = e2;
}

// insert d2 before d1
void dlist_insert_before(DList *d1, DList *d2) {
    //kprintf("%s(%p, %p)\n", __FUNCTION__, d1, d2);
    DList *e1 = d1->prev;
    DList *e2 = d2->prev;

    e1->next = d2;
    d2->prev = e1;
    e2->next = d1;
    d1->prev = e2;
}

// remove d from the list
void dlist_remove(DList *d) {
    //kprintf("%s(%p)\n", __FUNCTION__, d);
    d->prev->next = d->next;
    d->next->prev = d->prev;
    d->next = d;
    d->prev = d;
}

// push d2 to the front of the d1p list
void dlist_push(DList **d1p, DList *d2) {
    //kprintf("%s(%p, %p)\n", __FUNCTION__, d1p, d2);
    if (*d1p != NULL) {
        dlist_insert_before(*d1p, d2);
    }
    *d1p = d2;
}

// pop the front of the dp list
DList* dlist_pop(DList **dp) {
    //kprintf("%s(%p)\n", __FUNCTION__, dp);
    DList *d1 = *dp;
    DList *d2 = d1->next;
    dlist_remove(d1);
    if (d1 == d2) {
        *dp = NULL;
    } else {
        *dp = d2;
    }
    return d1;
}

// remove d2 from the list, advancing d1p if needed
void dlist_remove_from(DList **d1p, DList *d2) {
    //kprintf("%s(%p, %p)\n", __FUNCTION__, d1p, d2);
    if (*d1p == d2) {
        dlist_pop(d1p);
    } else {
        dlist_remove(d2);
    }
}