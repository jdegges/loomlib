/******************************************************************************
 * Copyright (c) 2010 Joey Degges
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *****************************************************************************/

#ifndef LOOMLIB_TREE_H
#define LOOMLIB_TREE_H

#include <stdbool.h>
#include <stddef.h>

/*
 * A tree.
 */
struct tree;

/*
 * A vertice.
 */
struct vertice;

/*
 * Create a new tree.
 * MAX_THREADS threads will be started.
 */
struct tree *
tree_new (size_t max_threads);

/*
 * Free a tree.
 * This will block until all of the threads have exited and there is no more
 * work to be done.
 */
void
tree_free (struct tree *tree);

bool
tree_terminate (struct tree *tree);


/*
 * Create a new vertice.
 * Root vertices will be passed NULL in PRODUCT
 * Non-Terminal vertices will be passed their parents return value in PRODUCT.
 *   All children will be passed the same pointer so they must lock sensitive
 *   areas appropriately.
 * Terminal verticies should return NULL since there are no children to handle
 *   the result (if non-null is returned then that memory will leak).
 */
struct vertice *
tree_new_vertice (void *(*routine)(void *data, void *product),
                  void *data);

/*
 * Free a vertice.
 * Once the tree is finished executing you must free all of the vertices to
 * avoid leaking memory.
 */
void
tree_free_vertice (struct vertice *vertice);

/*
 * Designate VERTICE to be the root of the tree.
 * Each tree may only have one root.
 */
bool
tree_add_root (struct tree *tree,
               struct vertice *root);

/*
 * Designate CHILD to be the child node of PARENT.
 * Each parent may have zero or more children.
 */
bool
tree_add_child (struct vertice *parent,
                struct vertice *child);

/*
 * Start running the tree.
 */
bool
tree_execute (struct tree *tree);

#endif
