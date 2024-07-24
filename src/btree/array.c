#include <core.h>
#include <foreach.h>
#include <interface.h>


static gboolean
nativeBTreeFilter(gpointer key, gpointer value, gpointer data)
{
  BTreeNode node = (BTreeNode) value;
  ForEachContext_t *ctxt = (ForEachContext_t *) data;
  napi_value accumulator = (napi_value) ctxt->data;
  napi_env env = ctxt->bTree->env;

  napi_value esNode, esKey, esValue, esIdx, cbResult;

  NAPI_CALL(env, false,
            napi_get_reference_value(env, node->esKeyValue, &esNode));

  NAPI_CALL(env, false, napi_get_named_property(env, esNode, KEY, &esKey));

  NAPI_CALL(env, false, napi_get_named_property(env, esNode, VALUE, &esValue));

  NAPI_CALL(env, false, napi_create_int64(env, ctxt->idx++, &esIdx));

  napi_value argv[] = {esValue, esKey, esIdx, ctxt->esbTree};

  NAPI_CALL(env, false,
            napi_call_function(env, ctxt->cbThis, ctxt->callback,
                               (sizeof(argv) / sizeof(napi_value)), argv,
                               &cbResult));

  NAPI_CALL(env, false, napi_coerce_to_bool(env, cbResult, &cbResult));

  bool ok = false;

  NAPI_CALL(env, false, napi_get_value_bool(env, cbResult, &ok));

  if (ok) {
    napi_handle_scope scope;

    NAPI_CALL(env, false, napi_open_handle_scope(env, &scope));

    napi_value box;

    // Create box es object
    NAPI_CALL(env, false, napi_create_object(env, &box));

    // Set key & value to box
    NAPI_CALL(env, false, napi_set_named_property(env, box, KEY, esKey));
    NAPI_CALL(env, false, napi_set_named_property(env, box, VALUE, esValue));

    nativeInsertNode(env, accumulator, box);

    NAPI_CALL(env, false, napi_close_handle_scope(env, scope));
    // ctxt->data = (void *) cbResult;
  }

  return false;
}

/**
 * Native callback for es reduce()
 */
static gboolean
nativeBTreeReduce(gpointer key, gpointer value, gpointer data)
{
  BTreeNode node = (BTreeNode) value;
  ForEachContext_t *ctxt = (ForEachContext_t *) data;
  napi_value accumulator = (napi_value) ctxt->data;
  napi_env env = ctxt->bTree->env;

  napi_value esNode, esKey, esValue, esIdx, cbResult;

  NAPI_CALL(env, false,
            napi_get_reference_value(env, node->esKeyValue, &esNode));

  NAPI_CALL(env, false, napi_get_named_property(env, esNode, KEY, &esKey));

  NAPI_CALL(env, false, napi_get_named_property(env, esNode, VALUE, &esValue));

  NAPI_CALL(env, false, napi_create_int64(env, ctxt->idx++, &esIdx));

  napi_value argv[] = {accumulator, esValue, esKey, esIdx, ctxt->esbTree};

  NAPI_CALL(env, false,
            napi_call_function(env, ctxt->cbThis, ctxt->callback,
                               (sizeof(argv) / sizeof(napi_value)), argv,
                               &cbResult));

  ctxt->data = (void *) cbResult;

  return false;
}

/**
 * Native callback for ES map()
 */
static gboolean
nativeBTreeMap(gpointer key, gpointer value, gpointer data)
{
  BTreeNode node = (BTreeNode) value;
  ForEachContext_t *ctxt = (ForEachContext_t *) data;
  napi_value array = (napi_value) ctxt->data;
  napi_env env = ctxt->bTree->env;

  napi_value esNode, esKey, esValue, esIdx, cbResult;

  NAPI_CALL(env, false,
            napi_get_reference_value(env, node->esKeyValue, &esNode));

  NAPI_CALL(env, false, napi_get_named_property(env, esNode, KEY, &esKey));

  NAPI_CALL(env, false, napi_get_named_property(env, esNode, VALUE, &esValue));

  NAPI_CALL(env, false, napi_create_int64(env, ctxt->idx, &esIdx));

  napi_value argv[] = {esValue, esKey, esIdx, ctxt->esbTree};

  NAPI_CALL(env, false,
            napi_call_function(env, ctxt->cbThis, ctxt->callback,
                               (sizeof(argv) / sizeof(napi_value)), argv,
                               &cbResult));

  NAPI_CALL(env, false, napi_set_element(env, array, ctxt->idx++, cbResult));

  return false;
}


napi_value
esMap(napi_env env, napi_callback_info cbInfo)
{
  napi_value esThis, array, callback, cbThis, argv[2];
  BTree_t *bTree;
  size_t argc = 2;

  // Get es this for current btree
  NAPI_CALL(env, false,
            napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  // Extract native BTree pointer
  EXTRACT_BTREE(env, esThis, bTree);

  CHECK_ARGC(1, msgTooFewArguments);
  callback = argv[0];

  if (argc > 1) {
    cbThis = argv[1];
  }
  else {
    NAPI_CALL(env, true, napi_get_global(env, &cbThis));
  }

  gint bTreeSize = g_tree_nnodes(bTree->nativeTree);

  NAPI_CALL(env, true,
            napi_create_array_with_length(env, (size_t) bTreeSize, &array));

  ForEachContext_t ctxt = {esThis, callback, cbThis, 0, bTree, array};

  g_tree_foreach(bTree->nativeTree, nativeBTreeMap, &ctxt);

  return array;
}

napi_value
esReduce(napi_env env, napi_callback_info cbInfo)
{
  napi_value esThis, callback, accumulator, cbThis, argv[2];
  BTree_t *bTree;
  size_t argc = 2;

  // Get es this for current btree
  NAPI_CALL(env, false,
            napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  // Extract native BTree pointer
  EXTRACT_BTREE(env, esThis, bTree);

  CHECK_ARGC(2, msgTooFewArguments);
  callback = argv[0];
  accumulator = argv[1];

  NAPI_CALL(env, true, napi_get_global(env, &cbThis));

  ForEachContext_t ctxt = {esThis, callback, cbThis, 0, bTree, accumulator};

  g_tree_foreach(bTree->nativeTree, nativeBTreeReduce, &ctxt);

  return (napi_value) ctxt.data;
}

napi_value
esFilter(napi_env env, napi_callback_info cbInfo)
{
  napi_value esThis, callback, accumulator, cbThis, argv[2];
  BTree_t *bTree;
  size_t argc = 2;

  // Get es this for current btree
  NAPI_CALL(env, false,
            napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  // Extract native BTree pointer
  EXTRACT_BTREE(env, esThis, bTree);

  CHECK_ARGC(1, msgTooFewArguments);
  callback = argv[0];

  if (argc > 1) {
    cbThis = argv[1];
  }
  else {
    NAPI_CALL(env, true, napi_get_global(env, &cbThis));
  }

  napi_value esbTreeConstructor, comparatorFunc;
  NAPI_CALL(
      env, true,
      napi_get_reference_value(env, btreeConstructorRef, &esbTreeConstructor));

  NAPI_CALL(env, true,
            napi_get_reference_value(env, bTree->comparator, &comparatorFunc));

  NAPI_CALL(env, true,
            napi_new_instance(env, esbTreeConstructor, 1, &comparatorFunc,
                              &accumulator));

  ForEachContext_t ctxt = {esThis, callback, cbThis, 0, bTree, accumulator};

  g_tree_foreach(bTree->nativeTree, nativeBTreeFilter, &ctxt);

  return (napi_value) ctxt.data;
}
