#ifndef __ITERATORS_H_
#define __ITERATORS_H_

#include <common.h>
#include <glib_local.h>
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
  GTreeNode_t currentNode;
  // ES value for return
  napi_value value;
} IteratorContext_t;

typedef struct {
  napi_value esbTree;
  napi_value callback;
  napi_value cbThis;
  size_t idx;
  BTree_t *bTree;
  gpointer data;
} ForEachContext_t;

typedef void (*forEachCallback)(ForEachContext_t *ctxt);

/**
 * Native callback for generic operations
 */
typedef void (*iteratorResultCallback)(IteratorContext_t *ctxt);


napi_value esForeach(napi_env env, napi_callback_info cbInfo);
napi_value esGenerator(napi_env env, napi_callback_info cbInfo);

void iteratorResultDefaultCb(IteratorContext_t *ctxt);
void iteratorResultValueCb(IteratorContext_t *ctxt);
void iteratorResultKeyCb(IteratorContext_t *ctxt);


#endif // __ITERATORS_H_
