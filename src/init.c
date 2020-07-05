#include "interface.h"

/**
 * Module initialization callback
 */
static napi_value init(napi_env env, napi_value exports) {
  napi_value esBTreeClass, symbolIterator;

  NAPI_GLOBAL_SYM(env, "iterator", symbolIterator);

  // Instance props
  napi_property_descriptor esBTreeProps[] = {
    // specific
    DEF_PROP("height", esHeight),

    // map
    DEF_PROP("size", esSize),
    DEF_METHOD("set", esSet),
    DEF_METHOD("get", esGet),
    DEF_METHOD("delete", esDelete),
    DEF_METHOD("clear", esClear),
    DEF_METHOD("has", esHas),

    // array
    DEF_METHOD("map", esMap),
    DEF_METHOD("reduce", esReduce),
    DEF_METHOD("filter", esFilter),

    // iterators
    DEF_METHOD("forEach", esForeach),
    DEF_METHOD_V_WITH_DATA(symbolIterator, esGenerator, (void *) iteratorResultDefaultCb),
    DEF_METHOD_WITH_DATA("entries", esGenerator, (void *) iteratorResultDefaultCb),
    DEF_METHOD_WITH_DATA("values", esGenerator, (void *) iteratorResultValueCb),
    DEF_METHOD_WITH_DATA("keys", esGenerator, (void *) iteratorResultKeyCb),

    // Extra methods
    DEF_METHOD("toMap", esToMap),
    DEF_METHOD("toSet", esToSet)
  };

  // Static props
  napi_property_descriptor staticProps[] = {
    DEF_METHOD("from", esStaticFrom)
  };


  NAPI_CALL(env, false,
    napi_define_class(env, "BTree", NAPI_AUTO_LENGTH, esConstructor, NULL, (sizeof(esBTreeProps) / sizeof(esBTreeProps[0])), esBTreeProps, &esBTreeClass));

  NAPI_CALL(env, false,
    napi_create_reference(env, esBTreeClass, 1, &constructor));

  NAPI_CALL(env, true,
    napi_define_properties(env, esBTreeClass, (sizeof(staticProps) / sizeof(staticProps)), staticProps));

  // export
  napi_property_descriptor props[] = {{
    "BTree",
    NULL,
    NULL,
    NULL,
    NULL,
    esBTreeClass,
    napi_default,
    NULL
  }};

  NAPI_CALL(env, false,
    napi_define_properties(env, exports, (sizeof(props) / sizeof(props[0])), props));

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);
