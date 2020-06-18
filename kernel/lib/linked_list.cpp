/**
 * @file linked_list.cpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief
 * @version 0.1
 * @date 2020-06-17
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 *
 */

#include <lib/linked_list.hpp>

/* Given a reference (pointer to pointer) to the head of a list
   and an int, inserts a new node on the front of the list. */
void push(struct px_double_node** head_ref, int new_data)
{
    struct px_double_node* new_node = NULL; //(struct px_double_node*)malloc(sizeof(struct px_double_node));
    new_node->data = new_data;
    new_node->next = (*head_ref);
    new_node->prev = NULL;
    if ((*head_ref) != NULL) {
        (*head_ref)->prev = new_node;
    }
    (*head_ref) = new_node;
}

/* Given a node as next_node, insert a new node before the given node */
void insertBefore(struct px_double_node** head_ref, struct px_double_node* next_node, int new_data)
{
    if (next_node == NULL) {
        px_kprint("the given next node cannot be NULL");
        return;
    }
    struct px_double_node* new_node = NULL; //(struct px_double_node*)malloc(sizeof(struct px_double_node));
    new_node->data = new_data;
    new_node->prev = next_node->prev;
    next_node->prev = new_node;
    new_node->next = next_node;
    if (new_node->prev != NULL) {
        new_node->prev->next = new_node;
    } else {
        (*head_ref) = new_node;
    }
}

/* Given a node as prev_node, insert a new node after the given node */
void insertAfter(struct px_double_node* prev_node, int new_data)
{
    if (prev_node == NULL) {
        px_kprint("the given previous node cannot be NULL");
        return;
    }
    struct px_double_node* new_node = NULL; //(struct px_double_node*)malloc(sizeof(struct px_double_node));
    new_node->data = new_data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
    new_node->prev = prev_node;
    if (new_node->next != NULL) {
        new_node->next->prev = new_node;
    }
}

/* Given a reference (pointer to pointer) to the head
   of a DLL and an int, appends a new node at the end  */
void append(struct px_double_node** head_ref, int new_data)
{
    struct px_double_node* new_node = NULL; //(struct px_double_node*)malloc(sizeof(struct px_double_node));
    struct px_double_node* last = *head_ref;
    new_node->data = new_data;
    new_node->next = NULL;
    if (*head_ref == NULL) {
        new_node->prev = NULL;
        *head_ref = new_node;
        return;
    }
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = new_node;
    new_node->prev = last;
    return;
}