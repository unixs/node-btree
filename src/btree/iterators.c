#include <iterators.h>

/**
 * Free iterator native data
 */
static void
freeIterator(napi_env env, void *finalize_data, void *finalize_hint) {
  g_free((gpointer) finalize_data);
}

inline static void
attachIteratorContext(napi_env env, napi_value esIterator, BTree_t *bTree) {
  // Alloc memory for native iterator context
  IteratorContext_t *c = g_new(IteratorContext_t, 1);

  c->bTree = bTree;
  c->state = ITERATOR_INIT;

  // Attach native data (context) to es value (iterator)
  NAPI_CALL(env, false,
    napi_wrap(env, esIterator, (gpointer) c, freeIterator, NULL, NULL));
}


/**
 * ES callback. Iterator next() method.
 */
static napi_value
esIteratorNext(napi_env env, napi_callback_info cbInfo) {
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
      itCtxt->currentNode = g_tree_node_first(itCtxt->bTree->nativeTree);
      break;

    case ITERATOR_LOOP:
      itCtxt->currentNode = g_tree_node_next(itCtxt->currentNode);
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
 * ES callback. bTree generator function.
 */
napi_value
esGenerator(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, esIterator, symIterator, generatorFn, symToStringTag,
    toStringTagValue, bind;
  BTree_t *bTree;
  gpointer data;

  // Get es this for current btree
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, &data));

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (gpointer *) &bTree));

  // Create es Iterator
  NAPI_CALL(env, false,
    napi_create_object(env, &esIterator));

  NAPI_GLOBAL_SYM(env, "iterator", symIterator);
  NAPI_GLOBAL_SYM(env, "toStringTag", symToStringTag);
  NAPI_CALL(env, true,
    napi_create_string_utf8(env, "BTreeIterator", NAPI_AUTO_LENGTH, &toStringTagValue));
  NAPI_CALL(env, true,
    napi_set_property(env, esIterator, symToStringTag, toStringTagValue));

  // Create next() iterator method
  napi_value nextFunction;
  NAPI_CALL(env, false,
    napi_create_function(env, "next", NAPI_AUTO_LENGTH, esIteratorNext, data, &nextFunction));
  NAPI_CALL(env, false,
    napi_set_named_property(env, esIterator, "next", nextFunction));
  // Create new generator
  NAPI_CALL(env, false,
    napi_create_function(env, "BTreeIterator", 0, esGenerator, data, &generatorFn));
  // Bind generator to btree this
  NAPI_CALL(env, true,
    napi_get_named_property(env, generatorFn, "bind", &bind));
  NAPI_CALL(env, true,
    napi_call_function(env, generatorFn, bind, 1, &esThis, &generatorFn));
  // Set generator fn
  NAPI_CALL(env, false,
    napi_set_property(env, esIterator, symIterator, generatorFn));

  attachIteratorContext(env, esIterator, bTree);

  return esIterator;
}

/**
 * Callback for iterator wich return key with value
 */
void
iteratorResultDefaultCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value esValue, tmp;

  BTreeNode node = (BTreeNode) g_tree_node_value(ctxt->currentNode);

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
void
iteratorResultValueCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value esValue;

  BTreeNode node = (BTreeNode) g_tree_node_value(ctxt->currentNode);

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esValue));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esValue, VALUE, &ctxt->value));
}

/**
 * Callback for iterator wich return key only
 */
void
iteratorResultKeyCb(IteratorContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value esValue;

  BTreeNode node = (BTreeNode) g_tree_node_value(ctxt->currentNode);

  NAPI_CALL(env, false,
    napi_get_reference_value(env, node->esKeyValue, &esValue));

  NAPI_CALL(env, false,
    napi_get_named_property(env, esValue, KEY, &ctxt->value));
}

napi_value
esForeachReverse(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, callback, cbThis, cbArgv[4], argv[2];
  size_t argc = 2;

  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  CHECK_ARGC(1, msgTooFewArguments);
  callback = argv[0];

  BTree_t *btree;
  EXTRACT_BTREE(env, esThis, btree);

  if (argc > 1) {
    cbThis = argv[1];
  }
  else {
    cbThis = getEsGlobal(env);
  }

  #ifdef HAS_GTREE_NODE
    GTreeNode *current = g_tree_node_last(btree->nativeTree);

    gint idx = 0;
    gint revIdx = g_tree_nnodes(btree->nativeTree);

    while (current) {
      cbArgv[0] = getNodeEsValue(env, current);
      cbArgv[1] = getNodeEsKey(env, current);

      NAPI_CALL(env, false,
        napi_create_int64(env, idx, &cbArgv[2]));

      idx++;
      revIdx--;

      NAPI_CALL(env, false,
        napi_create_int64(env, revIdx, &cbArgv[3]));

      NAPI_CALL(env, false,
        napi_call_function(
          env,
          cbThis,
          callback,
          (sizeof(cbArgv) / sizeof(napi_value)),
          cbArgv,
          NULL
        )
      );

      current = g_tree_node_previous(current);
    }
  #else
    GPtrArray *arr = gtreeToPtrArray(btree->nativeTree);

    napi_value key, value;
    napi_ref item ;
    for (glong revIdx = arr->len - 1, idx = 0; revIdx >= 0; revIdx--, idx++) {
      item = (napi_ref) g_ptr_array_index(arr, revIdx);

      unrefBtreeNodeEsObject(env, item, &key, &value);

      cbArgv[0] = value;
      cbArgv[1] = key;

      NAPI_CALL(env, false,
        napi_create_int64(env, idx, &cbArgv[2]));

      NAPI_CALL(env, false,
        napi_create_int64(env, revIdx, &cbArgv[3]));

      NAPI_CALL(env, false,
        napi_call_function(
          env,
          cbThis,
          callback,
          (sizeof(cbArgv) / sizeof(napi_value)),
          cbArgv,
          NULL
        )
      );
    }

    g_ptr_array_free(arr, TRUE);
  #endif

  return getEsUndef(env);
}

