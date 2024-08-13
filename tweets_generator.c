#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "markov_chain.h"

#define MAX_TWEET_LEN 20
#define BUFFER_SIZE 1000
#define DELIMITERS " \r\n\0"

static void print_word(const void *data) {
  printf(" %s", (const char *)data);
}

static int compare_words(const void *data1, const void *data2) {
  return strcmp((const char *)data1, (const char *)data2);
}

static void free_word(void *data) {
  free((char *)data);
}

static void *copy_word(const void *data) {
  char *word = (char *)data;
  char *copy = malloc(strlen(word) + 1);
  if (copy) {
    strcpy(copy, word);
  }
  return copy;
}

static bool is_word_last(const void *data) {
  char *word = (char *)data;
  return word[strlen(word) - 1] == '.';
}

MarkovChain *initialize_tweet_chain() {
  MarkovChain *chain = malloc(sizeof(MarkovChain));
  if (!chain) return NULL;

  chain->free_data = free_word;
  chain->print_func = print_word;
  chain->comp_func = compare_words;
  chain->copy_func = copy_word;
  chain->is_last = is_word_last;

  LinkedList *database = malloc(sizeof(LinkedList));
  if (!database) {
    free(chain);
    return NULL;
  }

  database->first = NULL;
  database->last = NULL;
  database->size = 0;
  chain->database = database;

  return chain;
}

static int fill_tweet_database(FILE *fp, int words_to_read, MarkovChain *chain) {
  char line[BUFFER_SIZE];
  int words_read = 0;
  Node *prev_node = NULL;

  while (fgets(line, sizeof(line), fp) && (words_to_read == -1 || words_read < words_to_read)) {
    char *word = strtok(line, DELIMITERS);
    while (word && (words_to_read == -1 || words_read < words_to_read)) {
      Node *curr_node = add_to_database(chain, word);
      if (!curr_node) return EXIT_FAILURE;

      if (prev_node && !chain->is_last(prev_node->data)) {
        if (add_node_to_counter_list(prev_node->data, curr_node->data, chain)) {
          return EXIT_FAILURE;
        }
      }

      prev_node = curr_node;
      word = strtok(NULL, DELIMITERS);
      words_read++;
    }
  }

  return EXIT_SUCCESS;
}

static void print_tweets(MarkovChain *chain, int num_of_tweets) {
  for (int i = 0; i < num_of_tweets; i++) {
    printf("Tweet %d:", i + 1);
    MarkovNode *start_node = get_first_random_node(chain);
    generate_random_sequence(chain, start_node, MAX_TWEET_LEN);
    printf("\n");
  }
}
