
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct bt {
  struct bnode *root;
  size_t size;
};

struct bnode {
  uint32_t n;

  int elems[2 * 2 + 1];
  struct bnode *children[2 * 2 + 2];
};

int bt_default_cmp(const int *a, const int *b);

struct bt bt_mk();
void bt_node_free(struct bnode *node);
void bt_free(struct bt bt);

ssize_t bt_node_bsearch(const struct bnode *node, const int *elem);

int *bt_lookup_node(const struct bt *bt, const int *elem, struct bnode **node);
int *bt_lookup(const struct bt *bt, const int *elem);

bool bt_insert(struct bt *bt, int elem, int *prev);
int bt_split_node(struct bnode *parent, size_t idx);
bool bt_node_insert(struct bnode *node, int elem, int *prev);
bool bt_remove(struct bt *bt, int *elem, int *removed);

void bt_print(struct bnode *node, int depth);
