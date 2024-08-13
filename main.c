#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "snake_ladder.h"
#include "tweet_generator.h"

#define SEED_INDEX 1
#define MODE_INDEX 2
#define ARGS_MODE_TWEETS 5
#define ARGS_MODE_SL 4
#define BASE 10

int main(int argc, char *argv[]) {
  if (argc != ARGS_MODE_TWEETS && argc != ARGS_MODE_SL) {
    printf("Usage: Incorrect number of arguments!\n");
    return EXIT_FAILURE;
  }

  int seed = strtol(argv[SEED_INDEX], NULL, BASE);
  srand(seed);

  if (strcmp(argv[MODE_INDEX], "tweets") == 0) {
    int num_of_tweets = strtol(argv[3], NULL, BASE);
    FILE *fp = fopen(argv[4], "r");
    if (!fp) {
      printf("Error: Unable to open file!\n");
      return EXIT_FAILURE;
    }

    MarkovChain *chain = initialize_tweet_chain();
    if (!chain) {
      printf("Error: Memory allocation failed!\n");
      fclose(fp);
      return EXIT_FAILURE;
    }

    if (fill_tweet_database(fp, -1, chain) == EXIT_FAILURE) {
      printf("Error: Failed to fill database!\n");
      free_markov_chain(&chain);
      fclose(fp);
      return EXIT_FAILURE;
    }

    print_tweets(chain, num_of_tweets);
    free_markov_chain(&chain);
    fclose(fp);

  } else if (strcmp(argv[MODE_INDEX], "snake_ladder") == 0) {
    int num_of_walks = strtol(argv[3], NULL, BASE);
    MarkovChain *chain = initialize_snake_ladder_chain();
    if (!chain) {
      printf("Error: Memory allocation failed!\n");
      return EXIT_FAILURE;
    }

    if (fill_snake_ladder_database(chain) == EXIT_FAILURE) {
      printf("Error: Failed to fill database!\n");
      free_markov_chain(&chain);
      return EXIT_FAILURE;
    }

    print_walks(chain, num_of_walks);
    free_markov_chain(&chain);

  } else {
    printf("Usage: Invalid mode selected!\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
