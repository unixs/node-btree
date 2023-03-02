#include <node_api.h>

#ifdef HAS_GTREE_NODE

  napi_value
  esToMap(napi_env env, napi_callback_info cbInfo);

  napi_value
  esToSet(napi_env env, napi_callback_info cbInfo);

  napi_value
  esToArrays(napi_env env, napi_callback_info cbInfo);

  napi_value
  esToArray(napi_env env, napi_callback_info cbInfo);

  napi_value
  esFlatten(napi_env env, napi_callback_info cbInfo);

  napi_value
  esKeys(napi_env env, napi_callback_info cbInfo);

  napi_value
  esValues(napi_env env, napi_callback_info cbInfo);

#endif
