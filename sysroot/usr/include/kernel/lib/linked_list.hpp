/**
 * @file linked_list.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief A simple linked list library.
 * @version 0.1
 * @date 2020-06-17
 *
 * @copyright Copyright Keeton Feavel et al (c) 2020
 *
 */

#include <sys/sys.hpp>

/* Node of a doubly linked list */
struct px_double_node {
    int data;
    struct px_double_node* next; // Pointer to next node in DLL
    struct px_double_node* prev; // Pointer to previous node in DLL
};

void push(struct px_double_node** head_ref, int new_data);
void insertBefore(struct px_double_node** head_ref, struct px_double_node* next_node, int new_data);
void insertAfter(struct px_double_node* prev_node, int new_data);
void append(struct px_double_node** head_ref, int new_data);