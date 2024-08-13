#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

#define ALLOCATION_MSG "Allocation failure: Failed to allocate new memory\n"


/***************************/
typedef struct MarkovNode MarkovNode;
typedef struct NextNodeCounter NextNodeCounter;
/***************************/




/***************************/
/*        STRUCTS          */
/***************************/

struct MarkovNode
{
    void *data;
    NextNodeCounter *counter_list;
    unsigned int node_appearances;
    unsigned int counter_list_size;
};

struct NextNodeCounter
{
    MarkovNode *markov_node;
    unsigned int frequency;
};
typedef void (*PrintFunc) (void const *);
typedef void (*FreeData) (void *);
typedef int (*CompFunc) (const void *, const void *);
typedef void * (*CopyFunc) (const void *);
typedef bool (*IsLast) (void const *);

typedef struct MarkovChain {
    LinkedList *database;
    // pointer to a func that receives data from a generic type and prints it
    // returns void.
    PrintFunc print_func;
    // a pointer to a function that gets a pointer of generic data type and
    //  frees it. returns void.
    FreeData free_data;
    // pointer to a func that gets 2 pointers of generic data type(same one)
    // and compare between them  returns :
    //a positive value if the first is bigger
    //a negative value if the second is bigger
    //0 if equal
    CompFunc comp_func;
    // a pointer to a function that  gets a pointer of generic data type and
    // returns a newly allocated copy of it
    // returns a generic pointer.
    CopyFunc copy_func;
    //a pointer to function that gets a pointer of generic data type and
    // returns:
    //      - true if it's the last state.
    //      - false otherwise.
    IsLast is_last;
} MarkovChain;


/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode* get_next_random_node(MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it.
 * The sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a
 * random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence(MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_markov_chain(MarkovChain **markov_chain);

/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_counter_list(MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);

/**
* Check if data_ptr is in database. If so, return the markov_node
 * wrapping it the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node* get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
* If data_ptr in markov_chain, return it's markov_node. Otherwise, create new
 * markov_node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return markov_node wrapping given data_ptr in given chain's database
 */
Node* add_to_database(MarkovChain *markov_chain, void *data_ptr);

MarkovNode *create_markov_node(MarkovChain *markov_chain, void *data_ptr);

#endif /* _MARKOV_CHAIN_H */
