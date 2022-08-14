#include <core.h>
#include <interface.h>


/**
 * ES callback. Return bTree height
 */
napi_value esHeight(napi_env env, napi_callback_info cbInfo) {
  napi_value esHeight;
  napi_value esThis;
  BTree_t *bTree;

  // Get es this
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, false,
    napi_unwrap(env, esThis, (void **) &bTree));

  // Native call to glib tree
  gint nativeHeight = g_tree_height(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env, false,
    napi_create_int64(env, nativeHeight, &esHeight));

  return esHeight;
}
