#include <core.h>
#include <interface.h>


napi_value
esToArrays(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, result;
  size_t argc = 0;

  NAPI_CALL(env, false, napi_get_cb_info(env, cbInfo, &argc, NULL, &esThis, NULL));

  BTree_t *btree;
  EXTRACT_BTREE(env, esThis, btree);

  GTreeNode *current = g_tree_node_first(btree->nativeTree);

  size_t arrSize = g_tree_nnodes(btree->nativeTree);

  NAPI_CALL(env, false,
    napi_create_array_with_length(env, arrSize, &result));

  napi_value item;
  size_t idx = 0;
  while (current) {
    item = getNodeEsFieldPair(env, current);
    NAPI_CALL(env, false,
      napi_set_element(env, result, idx++, item));
    current  = g_tree_node_next(current);
  }

  return result;
}

napi_value
esToArray(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, result;
  size_t argc = 0;

  NAPI_CALL(env, false, napi_get_cb_info(env, cbInfo, &argc, NULL, &esThis, NULL));

  BTree_t *btree;
  EXTRACT_BTREE(env, esThis, btree);

  GTreeNode *current = g_tree_node_first(btree->nativeTree);

  size_t arrSize = g_tree_nnodes(btree->nativeTree);

  NAPI_CALL(env, false,
    napi_create_array_with_length(env, arrSize, &result));

  napi_value item;
  size_t idx = 0;
  while (current) {
    item = getNodeEsObject(env, current);
    NAPI_CALL(env, false,
      napi_set_element(env, result, idx++, item));
    current  = g_tree_node_next(current);
  }

  return result;
}

napi_value
esFlatten(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, result;
  size_t argc = 0;

  NAPI_CALL(env, false, napi_get_cb_info(env, cbInfo, &argc, NULL, &esThis, NULL));

  BTree_t *btree;
  EXTRACT_BTREE(env, esThis, btree);

  GTreeNode *current = g_tree_node_first(btree->nativeTree);

  size_t arrSize = g_tree_nnodes(btree->nativeTree);

  NAPI_CALL(env, false,
    napi_create_array_with_length(env, arrSize, &result));

  napi_value item;
  size_t idx = 0;
  while (current) {
    item = getNodeEsKey(env, current);
    NAPI_CALL(env, false,
      napi_set_element(env, result, idx++, item));

    item = getNodeEsValue(env, current);
    NAPI_CALL(env, false,
      napi_set_element(env, result, idx++, item));

    current  = g_tree_node_next(current);
  }

  return result;
}

napi_value
esKeys(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, result;
  size_t argc = 0;

  NAPI_CALL(env, false, napi_get_cb_info(env, cbInfo, &argc, NULL, &esThis, NULL));

  BTree_t *btree;
  EXTRACT_BTREE(env, esThis, btree);

  GTreeNode *current = g_tree_node_first(btree->nativeTree);

  size_t arrSize = g_tree_nnodes(btree->nativeTree);

  NAPI_CALL(env, false,
    napi_create_array_with_length(env, arrSize, &result));

  napi_value item;
  size_t idx = 0;
  while (current) {
    item = getNodeEsKey(env, current);
    NAPI_CALL(env, false,
      napi_set_element(env, result, idx++, item));

    current  = g_tree_node_next(current);
  }

  return result;
}

napi_value
esValues(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, result;
  size_t argc = 0;

  NAPI_CALL(env, false, napi_get_cb_info(env, cbInfo, &argc, NULL, &esThis, NULL));

  BTree_t *btree;
  EXTRACT_BTREE(env, esThis, btree);

  GTreeNode *current = g_tree_node_first(btree->nativeTree);

  size_t arrSize = g_tree_nnodes(btree->nativeTree);

  NAPI_CALL(env, false,
    napi_create_array_with_length(env, arrSize, &result));

  napi_value item;
  size_t idx = 0;
  while (current) {
    item = getNodeEsValue(env, current);
    NAPI_CALL(env, false,
      napi_set_element(env, result, idx++, item));

    current  = g_tree_node_next(current);
  }

  return result;
}
