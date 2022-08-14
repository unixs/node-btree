#ifndef __ITERATORS_H_
#define __ITERATORS_H_

#include <core.h>
#include <utils.h>

/**
 * Native iterator state
 */
typedef enum ITERATOR_STATE {
  ITERATOR_INIT,
  ITERATOR_LOOP,
  ITERATOR_END
} IteratorState_t;

/**
 * ES iterator native context
 */
typedef struct {
  // Pointer to self tree
  BTree_t *bTree;
  // Saved iterator state
  IteratorState_t state;
  // Internal GLib BTree node
  GTreeNode *currentNode;
  // ES value for return
  napi_value value;
} IteratorContext_t;


typedef struct {
  BTree_t *bTree;
  gpointer data;
} IteratorIteratorData_t;


/**
 * Native callback for generic operations
 */
typedef void (*iteratorResultCallback)(IteratorContext_t *ctxt);


#endif // __ITERATORS_H_
