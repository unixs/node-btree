#include <static.h>
#include <iterators.h>


static void fromArrayCallback(ForEachContext_t *ctxt) {
  napi_env env = ctxt->bTree->env;
  napi_value key, value, box;
  bool valueIsArray = (bool) ctxt->data;

  if (valueIsArray) {
    NAPI_CALL(env, VOID_ON_FAIL,
      napi_get_element(env, ctxt->cbThis, 0, &key));

    NAPI_CALL(env, VOID_ON_FAIL,
      napi_get_element(env, ctxt->cbThis, 1, &value));
  }
  // Is Object
  else {
    NAPI_CALL(env, VOID_ON_FAIL,
      napi_get_named_property(env, ctxt->cbThis, KEY, &key));

    NAPI_CALL(env, VOID_ON_FAIL,
      napi_get_named_property(env, ctxt->cbThis, VALUE, &value));
  }

  NAPI_CALL(env, VOID_ON_FAIL,
    napi_create_object(env, &box));

  NAPI_CALL(env, VOID_ON_FAIL,
    napi_set_named_property(env, box, KEY, key));

  NAPI_CALL(env, VOID_ON_FAIL,
    napi_set_named_property(env, box, VALUE, value));

  nativeInsertNode(env, ctxt->esbTree, box);
}

static void iterableIterate(napi_env env, napi_value iterable, forEachCallback callback, ForEachContext_t *ctxt) {
  napi_value generator, SymbolIterator, iterator, next;

  NAPI_GLOBAL_SYM(env, "iterator", SymbolIterator);

  NAPI_CALL(env, VOID_ON_FAIL,
    napi_get_property(env, iterable,SymbolIterator, &generator));

  NAPI_CALL(env, VOID_ON_FAIL,
    napi_call_function(env, iterable, generator, 0, NULL, &iterator));

  NAPI_CALL(env, VOID_ON_FAIL,
    napi_get_named_property(env, iterator, "next", &next));

  bool isDone = true;
  bool valueIsArray = false;
  napi_value result, value, done;

  do {
    NAPI_CALL(env, VOID_ON_FAIL,
      napi_call_function(env, iterator, next, 0, NULL, &result));

    NAPI_CALL(env, VOID_ON_FAIL,
      napi_get_named_property(env, result, "done", &done));

    NAPI_CALL(env, VOID_ON_FAIL,
      napi_get_value_bool(env, done, &isDone));

    if (!isDone) {
      NAPI_CALL(env, VOID_ON_FAIL,
        napi_get_named_property(env, result, VALUE, &value));

      NAPI_CALL(env, VOID_ON_FAIL,
        napi_is_array(env, value, &valueIsArray));

      ctxt->cbThis = value;
      ctxt->data = (void *) valueIsArray;

      callback(ctxt);

      ctxt->idx++;
    }

  } while(!isDone);
}

static void fromKeyValueIterable(napi_env env, napi_value iterable, napi_value esbTree) {
  BTree_t *bTree;

  // Extract native BTree pointer
  EXTRACT_BTREE(env, esbTree, bTree);

  ForEachContext_t ctxt = {
    esbTree,
    NULL,
    NULL,
    0,
    bTree,
    NULL
  };

  iterableIterate(env, iterable, fromArrayCallback, &ctxt);
}


/**
 * static ES callback. BTree.from()
 */
napi_value esStaticFrom(napi_env env, napi_callback_info cbInfo) {
  napi_value result, BTreeConstructor, argv[2];
  napi_value global, Map;
  size_t argc = 2;

  // Get es arguments & context
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &BTreeConstructor, NULL));

  CHECK_ARGC(2, msgTooFewArguments);
  napi_value comparator = argv[0];
  napi_value iterable = argv[1];

  NAPI_CALL(env, true,
    napi_get_global(env, &global));
  NAPI_CALL(env, true,
    napi_get_named_property(env, global, "Map", &Map));

  NAPI_CALL(env, true,
    napi_new_instance(env, BTreeConstructor, 1, &comparator, &result));

  bool isPending;
  NAPI_CALL(env, true,
    napi_is_exception_pending(env, &isPending));

  if (isPending) {
    return NULL;
  }

  bool isArray = false;
  bool isMap = false;
  bool isGenericIterable = false;

  NAPI_CALL(env, true,
    napi_is_array(env, iterable, &isArray));

  NAPI_CALL(env, true,
    napi_instanceof(env, iterable, Map, &isMap));

  // May be generic?
  if (!(isArray || isMap)) {
    napi_handle_scope scope;

    NAPI_CALL(env, true,
      napi_open_handle_scope(env, &scope));

    // Duck typing. next() method exists?
    napi_value nextMethod;
    napi_valuetype nextMethodType;

    NAPI_CALL(env, true,
      napi_get_named_property(env, iterable, "next", &nextMethod));

    NAPI_CALL(env, true,
      napi_typeof(env, nextMethod, &nextMethodType));

    isGenericIterable = (nextMethodType == napi_function);

    NAPI_CALL(env, true,
      napi_close_handle_scope(env, scope));
  }

  if (isMap || isArray || isGenericIterable) {
    fromKeyValueIterable(env, iterable, result);
  }
  else {
    NAPI_CALL(env, true,
      napi_throw_error(env, NULL, "Second arg must be Array, Map or iterable"));
  }

  return result;
}
