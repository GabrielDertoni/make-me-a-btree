# Make Me a BTree

The basic idea of this library is to be simple to use and customize. Just
include the this file in your source code and set some macros and there you
go. It is also possible to generate diferent implementations of the btree for
each of the type needed.

## Usage

There are two main macros that should be defined in order to use this
library: BT_ELEM and BT_MKID. By defining these you should be able to get a
very customizable implementation of the btree for your specific type and
needs. For example, in order to create a btree of double (f64) one might do
something like:

```c
#define BT_ELEM       double
#define BT_MKID(name) f64_##name

#include "btree.h"
```

And would get a type `struct f64_bt` representing the btree and functions
with the same prefix like `f64_bt_insert` or `f64_bt_lookup`.

One top of that, one must specify a comparison function to use and define it
in `BT_CMP`. By default it uses the C comparison operators, so it will work
out of the box for primitive C types. Alternatively one can define `BT_LESS`
to be a function that returns true when one element compares less then the
other.

All of those macros will be undefined at the end of this header file.

In order to generate implementation and definitions in separate files. Just
include `btree.h` in the `.c` and in the header do the same, but also define
`BT_DECL_ONLY` to only generate definitions and no implementation.

In order to generate separate `bt.c` and `bt.h` files, simply run `make`. To
customize the genrated code, run `make 'DEFINES=-DBT_ELEM=<your_type>'`. More
`-D` flags can be added to further customize the implementation. Note that the
generated code may not work out of the box and may require some, but not much
modification. Maybe a couple of `#include` statements or something like that. To
specify a custom formatter use the flag `FORMATTER=clang-format` in the make
command, for example.

## Macros

| Macro                    | Default                      | Description                                        |
|--------------------------|------------------------------|----------------------------------------------------|
| BT_ELEM                  | int                          | Type of elements on the btree.                     |
| BT_MKID(name)            | name                         | Constructs a name.                                 |
| BT_MKFN(type, name, ...) | type MKID(name)(__VA_ARGS__) | Constructs a function signature.                   |
| BT_FACTOR                | 2                            | The branching factor.                              |
| BT_CMP                   | BT_MKID(bt_default_cmp)      | The comparison function.                           |
| BT_LESS                  | -                            | Compare less function.                             |
| BT_ELEM_FREE(elem)       | <empty>                      | Function to free an element of type `BT_ELEM`.     |
| BT_DECL_ONLY             | -                            | If defined, will not generate implementation.      |
| BT_ITER_STACK_SIZE       | 32                           | Iterator stack size (determines max size of tree). |
| BT_GENERATE              | -                            | When set, will not include any other file.         |

