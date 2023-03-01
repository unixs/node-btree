#include <node_api.h>

// Cached ES constructor
extern napi_ref btreeConstructorRef;

napi_value
esConstructor(napi_env env, napi_callback_info cbInfo);

napi_value
esConstructor(napi_env env, napi_callback_info cbInfo);

napi_value
esHeight(napi_env env, napi_callback_info cbInfo);

napi_value
esSet(napi_env env, napi_callback_info cbInfo);

napi_value
esGet(napi_env env, napi_callback_info cbInfo);

napi_value
esDelete(napi_env env, napi_callback_info cbInfo);

napi_value
esSize(napi_env env, napi_callback_info cbInfo);

napi_value
esClear(napi_env env, napi_callback_info cbInfo);

napi_value
esHas(napi_env env, napi_callback_info cbInfo);

napi_value
esMap(napi_env env, napi_callback_info cbInfo);

napi_value
esReduce(napi_env env, napi_callback_info cbInfo);

napi_value
esFilter(napi_env env, napi_callback_info cbInfo);

napi_value
esStaticFrom(napi_env env, napi_callback_info cbInfo);

napi_value
esForeach(napi_env env, napi_callback_info cbInfo);

napi_value
esForeachReverse(napi_env env, napi_callback_info cbInfo);

#include <interface_conversion.h>

#ifdef HAS_GTREE_NODE
  #include <intreface_iterators.h>
  #include <interface_search.h>
#endif
