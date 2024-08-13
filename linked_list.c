#include "linked_list.h"
#include <stdlib.h>

/**
 * Adds a new node to the linked list.
 * @param link_list The linked list.
 * @param data The data to store in the new node.
 * @return SUCCESS or FAILURE.
 */
int add(LinkedList *link_list, void *data) {
  Node *new_node = malloc(sizeof(Node));
  if (!new_node) return FAILURE;

  new_node->data = data;
  new_node->next = NULL;

  if (!link_list->first) {
    link_list->first = new_node;
  } else {
    link_list->last->next = new_node;
  }

  link_list->last = new_node;
  link_list->size++;

  return SUCCESS;
}
