/**
 * > BTree - a flexible implementatio of a btree in C.
 *
 * Author: Gabriel Dertoni - https://github.com/GabrielDertoni
 *
 * The basic idea of this library is to be simple to use and customize. Just
 * include the this file in your source code and set some macros and there you
 * go. It is also possible to generate diferent implementations of the btree for
 * each of the type needed.
 *
 * Usage
 * =====
 *
 * There are two main macros that should be defined in order to use this
 * library: BT_ELEM and BT_MKID. By defining these you should be able to get a
 * very customizable implementation of the btree for your specific type and
 * needs. For example, in order to create a btree of double (f64) one might do
 * something like:
 *
 * ```c
 * #define BT_ELEM       double
 * #define BT_MKID(name) f64_##name
 * #include "btree.h"
 * ```
 *
 * And would get a type `struct f64_bt` representing the btree and functions
 * with the same prefix like `f64_bt_insert` or `f64_bt_lookup`.
 *
 * One top of that, one must specify a comparison function to use and define it
 * in `BT_CMP`. By default it uses the C comparison operators, so it will work
 * out of the box for primitive C types. Alternatively one can define `BT_LESS`
 * to be a function that returns true when one element compares less then the
 * other.
 *
 * All of those macros will be undefined at the end of this header file.
 *
 * In order to generate implementation and definitions in separate files. Just
 * include `btree.h` in the `.c` and in the header do the same, but also define
 * `BT_DECL_ONLY` to only generate definitions and no implementation.
 *
 * Macros
 * ======
 *
 * Name                         Default                         Description
 * ----------------------------------------------------------------------------------------------------------------
 * BT_ELEM                      int                             Type of elements on the btree.
 * BT_MKID(name)                name                            Constructs a name.
 * BT_MKFN(type, name, ...)     type MKID(name)(__VA_ARGS__)    Constructs a function signature.
 * BT_FACTOR                    2                               The branching factor.
 * BT_CMP                       BT_MKID(bt_default_cmp)         The comparison function.
 * BT_LESS                      -                               Compare less function.
 * BT_ELEM_FREE(elem)           <empty>                         Function to free an element of type `BT_ELEM`.
 * BT_DECL_ONLY                 -                               If defined, will not generate implementation.
 * BT_ITER_STACK_SIZE           32                              Iterator stack size (determines max size of tree).
 * BT_GENERATE                  -                               When set, will not include any other file.
 */

#ifndef _BTREE_H_
#define _BTREE_H_

#ifndef BT_GENERATE

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#else

!#ifndef _BTREE_H_
!#define _BTREE_H_

!#include <stdio.h>
!#include <stdlib.h>
!#include <stdbool.h>
!#include <stdint.h>
!#include <string.h>
!#include <assert.h>

#endif

#ifndef BT_ELEM
#define BT_ELEM int
#endif

#ifndef BT_MKID
#define BT_MKID(name) name
#endif

#ifndef BT_FACTOR
#define BT_FACTOR 2
#endif

#ifndef BT_MKFN
#define BT_MKFN(type, name, ...) type BT_MKID(name)(__VA_ARGS__)
#endif

#ifndef BT_ELEM_FREE
#define BT_ELEM_FREE(elem)
#endif

struct BT_MKID(bt)
{
    struct BT_MKID(bnode)* root;
    size_t size;
};

struct BT_MKID(bnode)
{
    uint32_t n;
    // We allocate one more child and element in order to facilitate the split operation.
    BT_ELEM elems[2 * BT_FACTOR + 1];
    struct BT_MKID(bnode)* children[2 * BT_FACTOR + 2];
};

// Declarations

BT_MKFN(int, bt_default_cmp, const BT_ELEM* a, const BT_ELEM* b);

BT_MKFN(struct BT_MKID(bt), bt_mk,);
BT_MKFN(void, bt_node_free, struct BT_MKID(bnode)* node);
BT_MKFN(void, bt_free, struct BT_MKID(bt) bt);

// Binary searches for an element within a single node. If the element is found,
// return the index to that element. If it is not, return the negative of the
// index where the element would be inserted to maintain ordering minus one. So,
// if the element wasn't found because it is too small, -1 would be returned.
BT_MKFN(ssize_t, bt_node_bsearch, const struct BT_MKID(bnode)* node, const BT_ELEM* elem);

// Returns a pointer to the element if found. `node` and `offset` are set to the
// last node and child index respectively. When the function returns a valid
// pointer (not NULL), `node` will point to the last visited leaf node and
// `offset` will be the index where `elem` could be inserted in that node.
BT_MKFN(BT_ELEM*, bt_lookup_node, const struct BT_MKID(bt)* bt, const BT_ELEM* elem, struct BT_MKID(bnode)** node);

// Looks up `elem` in the tree. If it is contained, returns a reference to the
// element. If not, return `NULL`.
//
// NOTE: Changing the looked up element in any way that affects its ordering is
// a logic error.
BT_MKFN(BT_ELEM*, bt_lookup, const struct BT_MKID(bt)* bt, const BT_ELEM* elem);

// Inserts `elem` into the tree. Returns `true` if there was already another
// element in the tree that compares equal with `elem`. In that case, the value
// will be replaced and the old value will be put int `prev`. In case `prev` is
// null, the value will be freed. Otherwise the function returns `false`.
BT_MKFN(bool, bt_insert, struct BT_MKID(bt)* bt, BT_ELEM elem, BT_ELEM* prev);

// Splits the child node at `idx` of `parent` and modifies the `parent`s
// children array to fit the newly created node. This function will not look at
// any of the elements in the `elems` array of `parent`. Assumes that the child
// beeing split is full (has 2 * BT_FACTOR + 1 elements). Returns the promoted
// element.
BT_MKFN(BT_ELEM, bt_split_node, struct BT_MKID(bnode)* parent, size_t idx);

// Inserts `elem` into a btree of root `node`. Returns `true` if `elem` was
// already present in the tree and, in that case, `prev` will be overwritten
// with the replaced element from the tree.
BT_MKFN(bool, bt_node_insert, struct BT_MKID(bnode)* node, BT_ELEM elem, BT_ELEM* prev);

// TODO: Implement
BT_MKFN(bool, bt_remove, struct BT_MKID(bt)* bt, BT_ELEM* elem, BT_ELEM* removed);
// FIXME: Remove
BT_MKFN(void, bt_print, struct BT_MKID(bnode)* node, int depth);

#ifndef BT_DECL_ONLY

// Definitions

#ifndef BT_CMP
#define BT_CMP BT_MKID(bt_default_cmp)

#ifdef BT_LESS

BT_MKFN(int, bt_default_cmp, const BT_ELEM* a, const BT_ELEM* b)
{
    if (BT_LESS(b, a)) return  1;
    if (BT_LESS(a, b)) return -1;
    return 0;
}

#else

BT_MKFN(int, bt_default_cmp, const BT_ELEM* a, const BT_ELEM* b)
{
    if (*a > *b) return  1;
    if (*a < *b) return -1;
    return 0;
}

#endif
#endif

BT_MKFN(struct BT_MKID(bt), bt_mk,)
{
  return (struct BT_MKID(bt)) { .root = NULL };
}

BT_MKFN(void, bt_node_free, struct BT_MKID(bnode)* node)
{
    if (!node) return;
    for (size_t i = 0; i < node->n; i++)
    {
        BT_ELEM_FREE(node->elems[i]);
        BT_MKID(bt_node_free)(node->children[i]);
    }
    BT_MKID(bt_node_free)(node->children[node->n]);
    free(node);
}

BT_MKFN(void, bt_free, struct BT_MKID(bt) bt)
{
    BT_MKID(bt_node_free)(bt.root);
}

BT_MKFN(ssize_t, bt_node_bsearch, const struct BT_MKID(bnode)* node, const BT_ELEM* elem)
{
    // Binary search for the element in the current node.
    // NOTE: `curr->n` can't bet 0 because of the btree invariants.
    size_t left = 0;
    size_t right = node->n;
    size_t mid;
    int cmp;
    do
    {
        mid = left + (right - left) / 2;
        cmp = BT_CMP(elem, node->elems + mid);
        if      (cmp > 0) left  = mid + 1;
        else if (cmp < 0) right = mid;
    }
    while (cmp && left < right);

    // Found the element, return it.
    if (!cmp) return (ssize_t)mid;

    assert(left == right);
    return -(ssize_t)left - 1;
}

// Returns a pointer to the element if found. `node` and `offset` are set to the
// last node and child index respectively. When the function returns a valid
// pointer (not NULL), `node` will point to the last visited leaf node and
// `offset` will be the index where `elem` could be inserted in that node.
BT_MKFN(
    BT_ELEM*,
    bt_lookup_node,
    const struct BT_MKID(bt)* bt, const BT_ELEM* elem, struct BT_MKID(bnode)** node
) {
    struct BT_MKID(bnode)* curr = bt->root;
    while (curr)
    {
        // Assign to `*node`. At the end `*node` will point to the last visited node.
        if (node) *node = curr;
        ssize_t idx = BT_MKID(bt_node_bsearch)(curr, elem);
        if (idx >= 0) return curr->elems + idx;
        curr = curr->children[-idx - 1];
    }
    return NULL;
}

BT_MKFN(BT_ELEM*, bt_lookup, const struct BT_MKID(bt)* bt, const BT_ELEM* elem)
{
    return BT_MKID(bt_lookup_node)(bt, elem, NULL);
}

// Splits the child node at `idx` of `parent` and modifies the `parent`s
// children array to fit the newly created node. This function will not look at
// any of the elements in the `elems` array of `parent`. Assumes that the child
// beeing split is full (has 2 * BT_FACTOR + 1 elements). Returns the promoted
// element.
BT_MKFN(BT_ELEM, bt_split_node, struct BT_MKID(bnode)* parent, size_t idx)
{
#define SIZEOF_PTR sizeof(void*)

    struct BT_MKID(bnode)* child = parent->children[idx];

    // Points to right sibling of `child`.
    struct BT_MKID(bnode)** rchild = parent->children + idx + 1;

    // Move rest of children to the right to make space for the new child.
    memmove(rchild + 1, rchild, (parent->n - idx) * SIZEOF_PTR);

    // Allocate the split node sibling.
    *rchild = calloc(1, sizeof(struct BT_MKID(bnode)));

    // Move half of the elements to the sibling.
    memcpy((*rchild)->elems, child->elems + BT_FACTOR + 1, BT_FACTOR * sizeof(BT_ELEM));

    // If `child` is not a leaf (any of its children are not NULL), copy half of
    // its children to the new node.
    if (child->children[0])
        memcpy((*rchild)->children, child->children + BT_FACTOR + 1, (BT_FACTOR + 1) * SIZEOF_PTR);

    (*rchild)->n = BT_FACTOR;
    child->n     = BT_FACTOR;

    return child->elems[BT_FACTOR];

#undef SIZEOF_PTR
}

// Inserts `elem` into a btree of root `node`. Returns `true` if `elem` was
// already present in the tree and, in that case, `prev` will be overwritten
// with the replaced element from the tree.
BT_MKFN(bool, bt_node_insert, struct BT_MKID(bnode)* node, BT_ELEM elem, BT_ELEM* prev)
{
    ssize_t idx = BT_MKID(bt_node_bsearch)(node, &elem);

    if (idx >= 0)
    {
        if (prev) *prev = node->elems[idx];
        else BT_ELEM_FREE(node->elems[idx]);
        node->elems[idx] = elem;
        return true;
    }

    idx = -idx - 1;
    struct BT_MKID(bnode)* child = node->children[idx];

    // Check if `node` is a leaf
    if (child)
    {
        bool replaced = BT_MKID(bt_node_insert)(child, elem, prev);
        // The insertion did not overflow the child, it's ok to return.
        if (child->n <= 2 * BT_FACTOR) return replaced;

        // The promoted element is what we want to insert in this node (since
        // it's not a leaf).
        elem = BT_MKID(bt_split_node)(node, idx);
    }

    // Make space for the new element, and insert.
    BT_ELEM* p = node->elems + idx;
    memmove(p + 1, p, (node->n - idx) * sizeof(BT_ELEM));

    // Just insert the element (may be the original element beeing inserted or
    // the result of a promotion).
    *p = elem;
    node->n++;

    return false;
}

BT_MKFN(bool, bt_insert, struct BT_MKID(bt)* bt, BT_ELEM elem, BT_ELEM* prev)
{
    bool replaced = bt->root ? BT_MKID(bt_node_insert)(bt->root, elem, prev) : false;
    if (!bt->root || bt->root->n > 2 * BT_FACTOR)
    {
        struct BT_MKID(bnode) *new_root = calloc(1, sizeof(struct BT_MKID(bnode)));
        new_root->n            = 1;
        new_root->children[0]  = bt->root;
        new_root->elems[0]     = bt->root ? BT_MKID(bt_split_node)(new_root, 0) : elem;
        bt->root = new_root;
    }
    return replaced;
}

BT_MKFN(void, bt_print, struct BT_MKID(bnode)* node, int depth)
{
#define INDENT for (int __i = 0; __i < depth; __i++) printf("  ")
    if (!node)
    {
        INDENT;
        printf("<empty>\n");
        return;
    }

    INDENT;
    printf("elems:");
    for (int i = 0; i < node->n; i++)
        printf(" %d", node->elems[i]);
    printf("\n");

    if (!node->children[0]) return;
    for (int i = 0; i <= node->n; i++)
        BT_MKID(bt_print)(node->children[i], depth + 1);

#undef IDENT
}

#ifndef BT_ITER_STACK_SIZE
// Allows for (2 * BT_FACTOR)^32 elements max. Even if BT_FACTOR is 1,
// that's over 4M elements, which should be enough, if not, can always set
// BT_ITER_STACK_SIZE to something larger.
#define BT_ITER_STACK_SIZE 32
#endif

struct BT_MKID(bt_iter_frame) {
    size_t i;
    struct BT_MKID(bnode)* node;
};

// In-Order Depth First Search iterator
struct BT_MKID(bt_iter_dfs)
{
    size_t top;
    struct BT_MKID( bt_iter_frame ) stack[BT_ITER_STACK_SIZE];
};

BT_MKFN(struct BT_MKID(bt_iter_dfs), bt_iter_dfs_mk, struct BT_MKID(bt)* btree)
{
    return (struct BT_MKID(bt_iter_dfs)) {
        .top = 1,
        .stack = {
            [0] = {
                .i = 0,
                .node = btree->root
            }
        },
    };
}

BT_MKFN(BT_ELEM*, bt_iter_dfs_next, struct BT_MKID(bt_iter_dfs)* iter)
{
    // The frame pointer.
    struct BT_MKID(bt_iter_frame)* fp = iter->stack + iter->top - 1;
    bool visited_child = false;
    while (true)
    {
        if (!fp->node) return NULL;

        if (fp->i > fp->node->n)
        {
            // If already at the bottom of the stack, we're done! We don't want
            // to make the stack empty because in successive calls the caller
            // may expect to receive `NULL` every time.
            if (iter->top == 1) return NULL;

            // Pops a frame from the stack and yields a value if it can.
            iter->top--;
            fp--;

            // Mark that we just came back from visiting the ith child.
            visited_child = true;
        }
        else if (fp->node->children[fp->i] && !visited_child)
        {
            // Pushes a frame in the stack.
            struct BT_MKID(bt_iter_frame)* new_fp = fp + 1;
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

#endif

#endif

#ifdef BT_GENERATE
!#endif
#endif

#undef BT_ELEM
#undef BT_MKID
#undef BT_CMP
#undef BT_LESS
#undef BT_MKFN
#undef BT_FACTOR
#undef BT_DECL_ONLY
#undef BT_GENERATE

