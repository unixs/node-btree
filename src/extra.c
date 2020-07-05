#include "extra.h"

napi_value esToMap(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, map, global, constructor;
  size_t argc = 0;

  // Get es this for current btree
  NAPI_CALL(env, false, napi_get_cb_info(env, cbInfo, &argc, NULL, &esThis, NULL));

  NAPI_CALL(env, true,
    napi_get_global(env, &global));

  NAPI_CALL(env, true,
    napi_get_named_property(env, global, "Map", &constructor));

  NAPI_CALL(env, true,
    napi_new_instance(env, constructor, 1, &esThis, &map));

  return map;
}

napi_value esToSet(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, set, global, constructor, valuesFn, iterator;
  size_t argc = 0;

  // Get es this for current btree
  NAPI_CALL(env, false, napi_get_cb_info(env, cbInfo, &argc, NULL, &esThis, NULL));

  NAPI_CALL(env, true,
    napi_get_global(env, &global));

  NAPI_CALL(env, true,
    napi_get_named_property(env, global, "Set", &constructor));

  NAPI_CALL(env, true,
    napi_get_named_property(env, esThis, "values", &valuesFn));

  NAPI_CALL(env, true,
    napi_call_function(env, esThis, valuesFn, 0, NULL, &iterator));

  NAPI_CALL(env, true,
    napi_new_instance(env, constructor, 1, &iterator, &set));

  return set;
}
