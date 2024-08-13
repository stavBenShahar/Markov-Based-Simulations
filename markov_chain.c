#include <stdlib.h>
#include "markov_chain.h"
#include <stdbool.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE 1

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number Maximal number to return (not including).
 * @return Random number.
 */
int get_random_number(int max_number) {
  return rand() % max_number;
}

/**
 * Get a Markov-Node from the markov chain at a given index.
 * @param markov_chain The Markov chain.
 * @param index The index of the desired Markov-Node.
 * @return The MarkovNode at the specified index.
 */
MarkovNode *get_node_at_index(MarkovChain *markov_chain, int index) {
  Node *current = markov_chain->database->first;
  for (int i = 0; i < index; i++) {
    current = current->next;
  }
  return current->data;
}

/**
 * Get the first random Markov node that is not the last node in the sequence.
 * @param markov_chain The Markov chain.
 * @return The first random MarkovNode.
 */
MarkovNode *get_first_random_node(MarkovChain *markov_chain) {
  size_t chain_size = markov_chain->database->size;
  MarkovNode *node;

  do {
    unsigned int random_index = get_random_number(chain_size);
    node = get_node_at_index(markov_chain, random_index);
  } while (markov_chain->is_last(node->data));

  return node;
}

/**
 * Get the next random Markov node based on the current state.
 * @param current_node The current Markov node.
 * @return The next random MarkovNode.
 */
MarkovNode *get_next_random_node(MarkovNode *current_node) {
  unsigned int random_number = get_random_number(current_node->node_appearances);
  NextNodeCounter *next_node = current_node->counter_list;

  while (random_number >= next_node->frequency) {
    random_number -= next_node->frequency;
    next_node++;
  }

  return next_node->markov_node;
}

/**
 * Generates and prints a random sequence from the Markov chain.
 * @param markov_chain The Markov chain.
 * @param first_node The first node to start the sequence.
 * @param max_length The maximum length of the sequence.
 */
void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *first_node, int max_length) {
  MarkovNode *current_node = first_node ? first_node : get_first_random_node(markov_chain);
  int sequence_length = 1;

  while (!markov_chain->is_last(current_node->data) && sequence_length < max_length) {
    markov_chain->print_func(current_node->data);
    current_node = get_next_random_node(current_node);
    sequence_length++;
  }
  markov_chain->print_func(current_node->data);
}

/**
 * Frees the Markov chain and all its associated memory.
 * @param ptr_chain Pointer to the MarkovChain to free.
 */
void free_markov_chain(MarkovChain **ptr_chain) {
  if (!ptr_chain || !*ptr_chain) return;

  Node *current = (*ptr_chain)->database->first;
  while (current) {
    Node *next_node = current->next;
    free(current->data->counter_list);
    (*ptr_chain)->free_data(current->data->data);
    free(current->data);
    free(current);
    current = next_node;
  }

  free((*ptr_chain)->database);
  free(*ptr_chain);
  *ptr_chain = NULL;
}

/**
 * Adds a node to the counter list of another node, updating frequencies.
 * @param first_node The node whose counter list is being updated.
 * @param second_node The node to add to the counter list.
 * @return SUCCESS or FAILURE.
 */
bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode *second_node, MarkovChain *markov_chain) {
  if (!first_node->counter_list) {
    NextNodeCounter *new_counter_list = malloc(sizeof(NextNodeCounter));
    if (!new_counter_list) return FAILURE;

    new_counter_list->markov_node = second_node;
    new_counter_list->frequency = 1;

    first_node->counter_list = new_counter_list;
    first_node->counter_list_size = 1;
    first_node->node_appearances = 1;

    return SUCCESS;
  }

  for (unsigned int i = 0; i < first_node->counter_list_size; i++) {
    if (markov_chain->comp_func(first_node->counter_list[i].markov_node->data, second_node->data) == 0) {
      first_node->counter_list[i].frequency++;
      first_node->node_appearances++;
      return SUCCESS;
    }
  }

  size_t new_size = (first_node->counter_list_size + 1) * sizeof(NextNodeCounter);
  NextNodeCounter *new_counter_list = realloc(first_node->counter_list, new_size);
  if (!new_counter_list) return FAILURE;

  new_counter_list[first_node->counter_list_size].markov_node = second_node;
  new_counter_list[first_node->counter_list_size].frequency = 1;

  first_node->counter_list = new_counter_list;
  first_node->counter_list_size++;
  first_node->node_appearances++;

  return SUCCESS;
}

/**
 * Retrieves a node from the Markov chain's database.
 * @param markov_chain The Markov chain.
 * @param data_ptr The data to search for.
 * @return The node if found, otherwise NULL.
 */
Node *get_node_from_database(MarkovChain *markov_chain, void *data_ptr) {
  Node *current = markov_chain->database->first;

  while (current) {
    if (markov_chain->comp_func(current->data->data, data_ptr) == 0) {
      return current;
    }
    current = current->next;
  }

  return NULL;
}

/**
 * Adds a new node to the Markov chain's database if it doesn't already exist.
 * @param markov_chain The Markov chain.
 * @param data_ptr The data to add.
 * @return The node in the database, or NULL if allocation fails.
 */
Node *add_to_database(MarkovChain *markov_chain, void *data_ptr) {
  Node *existing_node = get_node_from_database(markov_chain, data_ptr);
  if (existing_node) return existing_node;

  MarkovNode *new_markov_node = create_markov_node(markov_chain, data_ptr);
  if (!new_markov_node) return NULL;

  if (add(markov_chain->database, new_markov_node) == FAILURE) {
    return NULL;
  }

  return markov_chain->database->last;
}

/**
 * Creates a new Markov node.
 * @param markov_chain The Markov chain.
 * @param data_ptr The data to store in the node.
 * @return The new Markov node, or NULL if allocation fails.
 */
MarkovNode *create_markov_node(MarkovChain *markov_chain, void *data_ptr) {
  void *data_copy = markov_chain->copy_func(data_ptr);
  if (!data_copy) return NULL;

  MarkovNode *new_node = malloc(sizeof(MarkovNode));
  if (!new_node) {
    free(data_copy);
    return NULL;
  }

  new_node->data = data_copy;
  new_node->counter_list = NULL;
  new_node->counter_list_size = 0;
  new_node->node_appearances = 0;

  return new_node;
}
