#include <node_api.h>

#define ES_CLASS_NAME "BTree"

// Cached ES constructor
extern napi_ref btreeConstructorRef;

napi_value
esConstructor(napi_env env, napi_callback_info cbInfo);

napi_value
esConstructor(napi_env env, napi_callback_info cbInfo);

napi_value
esToStringTag(napi_env env, napi_callback_info cbInfo);

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

#ifdef HAS_GTREE_NODE
#  include <interface_conversion.h>
#  include <intreface_iterators.h>
#  include <interface_search.h>
#endif
