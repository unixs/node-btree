#include "iterators.h"


/**
 * Free iterator native data
 */
static void freeIterator(napi_env env, void *finalize_data, void *finalize_hint) {
  g_free((gpointer) finalize_data);
}


/**
 * Native forEach() callback
 */
static gboolean nativeBTreeForEach(gpointer key, gpointer val, gpointer data) {
  BTreeNode node = (BTreeNode) val;
  ForEachContext_t *ctxt = (ForEachContext_t *) data;
  napi_env env = ctxt->bTree->env;
  napi_value esObject, esKey, esValue, esIdx, esNull;

  if (val == NULL) {
    NAPI_CALL(env, false,
      napi_throw_error(env, NULL, msgCorrupt));

    return TRUE;
  }

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esObject));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esObject, KEY, &esKey));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esObject, VALUE, &esValue));

  NAPI_CALL(env, false,
    napi_create_int64(env, ctxt->idx, &esIdx));

  napi_value argv[] = {
    esValue,
    esKey,
    esIdx
  };

  NAPI_CALL(env, false,
    napi_get_null(env, &esNull));

  NAPI_CALL(env, false,
    napi_call_function(env, ctxt->cbThis, ctxt->callback, (sizeof(argv) / sizeof(napi_value)), argv, NULL));

  ctxt->idx++;

  return FALSE;
}

/**
 * ES callback. Iterator next() method.
 */
static napi_value esIteratorNext(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  IteratorContext_t *itCtxt;
  iteratorResultCallback resultCb;

  // Get es this for current btree
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, (void **) &resultCb));

  // Extract native pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &itCtxt));

  switch (itCtxt->state) {
    case ITERATOR_INIT:
      itCtxt->state = ITERATOR_LOOP;
      itCtxt->currentNode = local_g_tree_first_node(itCtxt->bTree->nativeTree);
      break;

    case ITERATOR_LOOP:
      itCtxt->currentNode = local_g_tree_node_next(itCtxt->currentNode);
      break;

    case ITERATOR_END:
      // noop
      break;
  }

  if (itCtxt->currentNode == NULL) {
    itCtxt->state = ITERATOR_END;
  }

  // Prepare result:

  napi_value esIteratorResult, isDone;
  NAPI_CALL(env, false,
    napi_create_object(env, &esIteratorResult));

  if (itCtxt->state == ITERATOR_END) {
    NAPI_CALL(env, false,
      napi_get_undefined(env, &itCtxt->value));

    NAPI_CALL(env, false,
      napi_get_boolean(env, true, &isDone));
  }
  else {
    resultCb(itCtxt);
    NAPI_CALL(env, false,
      napi_get_boolean(env, false, &isDone));
  }

  NAPI_CALL(env, false,
    napi_set_named_property(env, esIteratorResult, VALUE, itCtxt->value));

  NAPI_CALL(env, false,
    napi_set_named_property(env, esIteratorResult, "done", isDone));

  return esIteratorResult;
}


/**
 * ES callback. es forEach() method
 */
napi_value esForeach(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, undefined, callback, cbThis, argv[2];
  BTree_t *bTree;
  size_t argc = 2;


  // Get es this for current btree
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  CHECK_ARGC(1, msgTooFewArguments);
  callback = argv[0];

  if (argc > 1) {
    cbThis = argv[1];
  }
  else {
    NAPI_CALL(env, true,
      napi_get_global(env, &cbThis));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  ForEachContext_t ctxt = {
    esThis,
    callback,
    cbThis,
    0,
    bTree,
    NULL
  };

  g_tree_foreach(bTree->nativeTree, nativeBTreeForEach, &ctxt);

  NAPI_CALL(env, false,
    napi_get_undefined(env, &undefined));

  return undefined;
}

/**
 * ES callback. bTree generator function.
 */
napi_value esGenerator(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, esIterator;
  BTree_t *bTree;
  void *data;

  // Get es this for current btree
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, &data));

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Create es Iterator
  NAPI_CALL(env, false,
    napi_create_object(env, &esIterator));

  // Create next() iterator method
  napi_value nextFunction;
  NAPI_CALL(env, false,
    napi_create_function(env, "next", NAPI_AUTO_LENGTH, esIteratorNext, data, &nextFunction));
  NAPI_CALL(env, false,
    napi_set_named_property(env, esIterator, "next", nextFunction));

  // Alloc memory for native iterator context
  IteratorContext_t *itCtxt = g_new(IteratorContext_t, 1);
  itCtxt->bTree = bTree;
  itCtxt->state = ITERATOR_INIT;

  // Attach native data (context) to es value (iterator)
  NAPI_CALL(env, false,
    napi_wrap(env, esIterator, (void *) itCtxt, freeIterator, NULL, NULL));

  return esIterator;
}

/**
 * Callback for iterator wich return key with value
 */
void iteratorResultDefaultCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value esValue, tmp;

  BTreeNode node = (BTreeNode) local_g_tree_node_value(ctxt->currentNode);

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esValue));

  NAPI_CALL(env, false,
    napi_create_array_with_length(env, 2, &ctxt->value));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esValue, KEY, &tmp));
  NAPI_CALL(env, false,
    napi_set_element(env, ctxt->value, 0, tmp));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esValue, VALUE, &tmp));
  NAPI_CALL(env, false,
    napi_set_element(env, ctxt->value, 1, tmp));
}

/**
 * Callback for iterator wich return value only
 */
void iteratorResultValueCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value esValue;

  BTreeNode node = (BTreeNode) local_g_tree_node_value(ctxt->currentNode);

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esValue));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esValue, VALUE, &ctxt->value));
}

/**
 * Callback for iterator wich return key only
 */
void iteratorResultKeyCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value esValue;

  BTreeNode node = (BTreeNode) local_g_tree_node_value(ctxt->currentNode);

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esValue));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esValue, KEY, &ctxt->value));
}
