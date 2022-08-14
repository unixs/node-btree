#include <comparator.h>

gint
nativeComparator(gconstpointer a, gconstpointer b, gpointer bTree) {
  int64_t compareResult = 0;

  BTree_t *bTreeWrap = (BTree_t *) bTree;
  BTreeNode nodeA = (BTreeNode) a;
  BTreeNode nodeB = (BTreeNode) b;

  napi_env env = bTreeWrap->env;
  napi_value boxA, boxB, keyA, keyB, esNull, esResult, comparator;

  NAPI_CALL(env, false,
    napi_get_reference_value(env, nodeA->esKeyValue, &boxA));

  NAPI_CALL(env, false,
    napi_get_reference_value(env, nodeB->esKeyValue, &boxB));

  NAPI_CALL(env, false,
    napi_get_named_property(env, boxA, KEY, &keyA));

  NAPI_CALL(env, false,
    napi_get_named_property(env, boxB, KEY, &keyB));

  NAPI_CALL(env, false,
    napi_get_null(env, &esNull));

  NAPI_CALL(env, false,
    napi_get_reference_value(env, bTreeWrap->comparator, &comparator));

  napi_value argv[] = { keyA, keyB };
  NAPI_CALL(env, false,
    napi_call_function(env, esNull, comparator, 2, argv, &esResult));

  NAPI_CALL(env, false,
    napi_get_value_int64(env, esResult, &compareResult));

  return (gint) compareResult;
}

