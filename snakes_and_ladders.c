#include "markov_chain.h"
#include <stdlib.h>
#include <stdio.h>

#define BOARD_SIZE 100
#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20
#define EMPTY -1

typedef struct Cell {
    int number;
    int ladder_to;
    int snake_to;
} Cell;

const int transitions[][2] = {
    {13, 4}, {85, 17}, {95, 67}, {97, 58}, {66, 89},
    {87, 31}, {57, 83}, {91, 25}, {28, 50}, {35, 11},
    {8,  30}, {41, 62}, {81, 43}, {69, 32}, {20, 39},
    {33, 70}, {79, 99}, {23, 76}, {15, 47}, {61, 14}
};

static int create_board(Cell *cells[BOARD_SIZE]) {
  for (int i = 0; i < BOARD_SIZE; i++) {
    cells[i] = malloc(sizeof(Cell));
    if (!cells[i]) {
      for (int j = 0; j < i; j++) {
        free(cells[j]);
      }
      return EXIT_FAILURE;
    }
    *cells[i] = (Cell) {i + 1, EMPTY, EMPTY};
  }

  for (int i = 0; i < NUM_OF_TRANSITIONS; i++) {
    int from = transitions[i][0];
    int to = transitions[i][1];
    if (from < to) {
      cells[from - 1]->ladder_to = to;
    } else {
      cells[from - 1]->snake_to = to;
    }
  }

  return EXIT_SUCCESS;
}

int fill_snake_ladder_database(MarkovChain *markov_chain) {
  Cell *cells[BOARD_SIZE];
  if (create_board(cells) == EXIT_FAILURE) {
    return EXIT_FAILURE;
  }

  for (int i = 0; i < BOARD_SIZE; i++) {
    add_to_database(markov_chain, cells[i]);
  }

  for (int i = 0; i < BOARD_SIZE; i++) {
    MarkovNode *from_node = get_node_from_database(markov_chain, cells[i])->data;
    MarkovNode *to_node = NULL;

    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY) {
      int target_index = (cells[i]->ladder_to != EMPTY ? cells[i]->ladder_to : cells[i]->snake_to) - 1;
      to_node = get_node_from_database(markov_chain, cells[target_index])->data;
      add_node_to_counter_list(from_node, to_node, markov_chain);
    } else {
      for (int j = 1; j <= DICE_MAX; j++) {
        int target_index = cells[i]->number + j - 1;
        if (target_index >= BOARD_SIZE) break;
        to_node = get_node_from_database(markov_chain, cells[target_index])->data;
        add_node_to_counter_list(from_node, to_node, markov_chain);
      }
    }
  }

  for (int i = 0; i < BOARD_SIZE; i++) {
    free(cells[i]);
  }

  return EXIT_SUCCESS;
}

static void print_step(const void *cell) {
  const Cell *c = (const Cell *)cell;
  if (c->ladder_to != EMPTY) {
    printf("[%d]-ladder to %d -> ", c->number, c->ladder_to);
  } else if (c->snake_to != EMPTY) {
    printf("[%d]-snake to %d -> ", c->number, c->snake_to);
  } else if (c->number == LAST_CELL) {
    printf("[%d]", c->number);
  } else {
    printf("[%d] -> ", c->number);
  }
}

static bool is_cell_last(const void *data) {
  const Cell *c = (const Cell *)data;
  return c->number == LAST_CELL;
}

static void free_cell_data(void *data) {
  free((Cell *)data);
}

static int compare_cells(const void *cell1, const void *cell2) {
  const Cell *c1 = (const Cell *)cell1;
  const Cell *c2 = (const Cell *)cell2;
  return c1->number - c2->number;
}

static void *copy_cell(const void *data) {
  const Cell *original = (const Cell *)data;
  Cell *copy = malloc(sizeof(Cell));
  if (copy) {
    memcpy(copy, original, sizeof(Cell));
  }
  return copy;
}

MarkovChain *initialize_snake_ladder_chain() {
  MarkovChain *chain = malloc(sizeof(MarkovChain));
  if (!chain) return NULL;

  chain->free_data = free_cell_data;
  chain->print_func = print_step;
  chain->comp_func = compare_cells;
  chain->copy_func = copy_cell;
  chain->is_last = is_cell_last;

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
