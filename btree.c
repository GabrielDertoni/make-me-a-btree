
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

struct bt
{
    struct bnode* root;
    size_t size;
};

struct bnode
{
    uint32_t n;

    int elems[2 * 2 + 1];
    struct bnode* children[2 * 2 + 2];
};

int bt_default_cmp(const int* a, const int* b);

struct bt bt_mk();
void bt_node_free(struct bnode* node);
void bt_free(struct bt bt);

ssize_t bt_node_bsearch(const struct bnode* node, const int* elem);

int* bt_lookup_node(const struct bt* bt, const int* elem, struct bnode** node);
int* bt_lookup(const struct bt* bt, const int* elem);

bool bt_insert(struct bt* bt, int elem, int* prev);
int bt_split_node(struct bnode* parent, size_t idx);
bool bt_node_insert(struct bnode* node, int elem, int* prev);
bool bt_remove(struct bt* bt, int* elem, int* removed);

void bt_print(struct bnode* node, int depth);

int bt_default_cmp(const int* a, const int* b)
{
    if (*a > *b) return 1;
    if (*a < *b) return -1;
    return 0;
}
struct bt bt_mk()
{
  return (struct bt) { .root = NULL };
}

void bt_node_free(struct bnode* node)
{
    if (!node) return;
    for (size_t i = 0; i < node->n; i++)
    {
        ;
        bt_node_free(node->children[i]);
    }
    bt_node_free(node->children[node->n]);
    free(node);
}

void bt_free(struct bt bt)
{
    bt_node_free(bt.root);
}

ssize_t bt_node_bsearch(const struct bnode* node, const int* elem)
{
    size_t left = 0;
    size_t right = node->n;
    size_t mid;
    int cmp;
    do
    {
        mid = left + (right - left) / 2;
        cmp = bt_default_cmp(elem, node->elems + mid);
        if (cmp > 0) left = mid + 1;
        else if (cmp < 0) right = mid;
    }
    while (cmp && left < right);
    if (!cmp) return (ssize_t)mid;

    assert(left == right);
    return -(ssize_t)left - 1;
}

int* bt_lookup_node(const struct bt* bt, const int* elem, struct bnode** node)

  {
    struct bnode* curr = bt->root;
    while (curr)
    {

        if (node) *node = curr;
        ssize_t idx = bt_node_bsearch(curr, elem);
        if (idx >= 0) return curr->elems + idx;
        curr = curr->children[-idx - 1];
    }
    return NULL;
}

int* bt_lookup(const struct bt* bt, const int* elem)
{
    return bt_lookup_node(bt, elem, NULL);
}
int bt_split_node(struct bnode* parent, size_t idx)
{
    struct bnode* child = parent->children[idx];
    struct bnode** rchild = parent->children + idx + 1;
    memmove(rchild + 1, rchild, (parent->n - idx) * sizeof(void*));
    *rchild = calloc(1, sizeof(struct bnode));
    memcpy((*rchild)->elems, child->elems + 2 + 1, 2 * sizeof(int));

    if (child->children[0])
        memcpy((*rchild)->children, child->children + 2 + 1, (2 + 1) * sizeof(void*));

    (*rchild)->n = 2;
    child->n = 2;

    return child->elems[2];
}
bool bt_node_insert(struct bnode* node, int elem, int* prev)
{
    ssize_t idx = bt_node_bsearch(node, &elem);

    if (idx >= 0)
    {
        if (prev) *prev = node->elems[idx];
        else ;
        node->elems[idx] = elem;
        return true;
    }

    idx = -idx - 1;
    struct bnode* child = node->children[idx];
    if (child)
    {
        bool replaced = bt_node_insert(child, elem, prev);

        if (child->n <= 2 * 2) return replaced;

        elem = bt_split_node(node, idx);
    }
    int* p = node->elems + idx;
    memmove(p + 1, p, (node->n - idx) * sizeof(int));

    *p = elem;
    node->n++;

    return false;
}

bool bt_insert(struct bt* bt, int elem, int* prev)
{
    bool replaced = bt->root ? bt_node_insert(bt->root, elem, prev) : false;
    if (!bt->root || bt->root->n > 2 * 2)
    {
        struct bnode *new_root = calloc(1, sizeof(struct bnode));
        new_root->n = 1;
        new_root->children[0] = bt->root;
        new_root->elems[0] = bt->root ? bt_split_node(new_root, 0) : elem;
        bt->root = new_root;
    }
    return replaced;
}

void bt_print(struct bnode* node, int depth)
{

    if (!node)
    {
        for (int __i = 0; __i < depth; __i++) printf("  ");
        printf("<empty>\n");
        return;
    }

    for (int __i = 0; __i < depth; __i++) printf("  ");
    printf("elems:");
    for (int i = 0; i < node->n; i++)
        printf(" %d", node->elems[i]);
    printf("\n");

    if (!node->children[0]) return;
    for (int i = 0; i <= node->n; i++)
        bt_print(node->children[i], depth + 1);
}

struct bt_iter_frame {
    size_t i;
    struct bnode* node;
};
struct bt_iter_dfs
{
    size_t top;
    struct bt_iter_frame stack[32];
};

struct bt_iter_dfs bt_iter_dfs_mk(struct bt* btree)
{
    return (struct bt_iter_dfs) {
        .top = 1,
        .stack = {
            [0] = {
                .i = 0,
                .node = btree->root
            }
        },
    };
}

int* bt_iter_dfs_next(struct bt_iter_dfs* iter)
{

    struct bt_iter_frame* fp = iter->stack + iter->top - 1;
    bool visited_child = false;
    while (true)
    {
        if (!fp->node) return NULL;

        if (fp->i > fp->node->n)
        {

            if (iter->top == 1) return NULL;
            iter->top--;
            fp--;
            visited_child = true;
        }
        else if (fp->node->children[fp->i] && !visited_child)
        {

            struct bt_iter_frame* new_fp = fp + 1;
            iter->top++;
            new_fp->i = 0;
            new_fp->node = fp->node->children[fp->i];
            fp = new_fp;
        }
        else if (fp->i < fp->node->n)
        {
            return fp->node->elems + fp->i++;
        }
        else
        {
            fp->i++;
        }
    }
}
