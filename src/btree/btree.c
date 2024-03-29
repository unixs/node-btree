#include <export.h>

#include <interface.h>
#include <utils.h>

napi_ref btreeConstructorRef;

napi_property_descriptor btree_export(napi_env env) {
  napi_value esBTreeClass;

  #ifdef HAS_GTREE_NODE
    napi_value symbolIterator;
    NAPI_GLOBAL_SYM(env, "iterator", symbolIterator);
  #endif

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
    DEF_METHOD("forEachReverse", esForeachReverse),
    // Iterators are possible with GLib version >= 2.68 only
    #ifdef HAS_GTREE_NODE
      DEF_METHOD_V_WITH_DATA(symbolIterator, esGenerator, (void *) iteratorResultDefaultCb),
      DEF_METHOD_WITH_DATA("entries", esGenerator, (void *) iteratorResultDefaultCb),
      DEF_METHOD_WITH_DATA("values", esGenerator, (void *) iteratorResultValueCb),
      DEF_METHOD_WITH_DATA("keys", esGenerator, (void *) iteratorResultKeyCb),
    #endif

    // Extra methods
    #ifdef HAS_GTREE_NODE
      DEF_METHOD("toMap", esToMap),
      DEF_METHOD("toSet", esToSet),
      DEF_METHOD("toArrays", esToArrays),
      DEF_METHOD("toArray", esToArray),
      DEF_METHOD("flatten", esFlatten),
      DEF_METHOD("getKeys", esKeys),
      DEF_METHOD("getValues", esValues),
      DEF_METHOD("first", esFirst),
      DEF_METHOD("last", esLast),
      DEF_METHOD("before", esBefore),
      DEF_METHOD("after", esAfter),
      DEF_METHOD("between", esBetween),
    #endif
  };

  // Static props
  napi_property_descriptor staticProps[] = {
    DEF_METHOD("from", esStaticFrom)
  };

  NAPI_CALL(env, false,
    napi_define_class(env, "BTree", NAPI_AUTO_LENGTH, esConstructor, NULL, (sizeof(esBTreeProps) / sizeof(esBTreeProps[0])), esBTreeProps, &esBTreeClass));

  NAPI_CALL(env, false,
    napi_create_reference(env, esBTreeClass, 1, &btreeConstructorRef));

  NAPI_CALL(env, false,
    napi_define_properties(env, esBTreeClass, (sizeof(staticProps) / sizeof(staticProps)), staticProps));

  napi_property_descriptor descriptor = {
    "BTree",
    NULL,
    NULL,
    NULL,
    NULL,
    esBTreeClass,
    napi_default,
    NULL
  };

  return descriptor ;
}
