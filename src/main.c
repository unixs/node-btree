#include <stdio.h>
#include <stdlib.h>
#include <node_api.h>
#include "include/common.h"
#include <glib.h>

typedef struct {
  napi_value comparator
} BTree_t;

gint nativeComparator(gconstpointer a, gconstpointer b) {
  return 0;
}

napi_value init(napi_env env, napi_value exports);

napi_ref constructor;

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);

napi_value __hello(napi_env env, napi_callback_info info) {
  napi_value str;

  NAPI_CALL(env,
    napi_create_string_utf8(env, "hello", NAPI_AUTO_LENGTH, &str)
  );

  return str;
}

napi_value BTreeConstructor(napi_env env, napi_callback_info cbInfo) {
  printf("Constructor called.\n");

  napi_value esBtree;
  napi_ref ref;

  size_t argc = 1;
  napi_value args[1];
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, args, &esBtree, NULL));

  BTree_t *bTree = (BTree_t *) malloc(sizeof(BTree_t));
  bTree->comparator = args[0];

  napi_valuetype comparatorType;
  NAPI_CALL(env, napi_typeof(env, bTree->comparator, &comparatorType));

  if (comparatorType != napi_function) {
    NAPI_CALL(env, napi_throw_error(env, "10", "First arg must be comparator function"));
  }

  // Define comparator as not enumerable & ro property of es btree instance
  napi_property_descriptor comparatorProp = {
    "comparator",
    NULL,

    NULL,
    NULL,
    NULL,
    bTree->comparator,

    napi_default,
    NULL
  };
  NAPI_CALL(env, napi_define_properties(env, esBtree, 1, &comparatorProp));

  GTree *nativeTree = g_tree_new_with_data(nativeComparator, bTree);

  // NAPI_CALL(env, napi_wrap(env, instance, native, NULL, NULL, &ref));

  // BUG: comparator die here
  return esBtree;
}

void initGlobals(napi_env env) {
  napi_value globl;
  NAPI_CALL(env, napi_get_global(env, &globl));

  napi_value fake;
  NAPI_CALL(env, napi_create_int64(env, 100500, &fake));

  napi_value BTreeClass;

  NAPI_CALL(env, napi_define_class(env, "BTree", NAPI_AUTO_LENGTH, BTreeConstructor, NULL, 0, NULL, &BTreeClass));

  NAPI_CALL(env, napi_create_reference(env, BTreeClass, 1, &constructor));

  napi_property_descriptor desc[] = {
    {
      "int64_t",
      NULL,

      NULL,
      NULL,
      NULL,
      fake,

      napi_default,
      NULL
    },
    {
      "BTree",
      NULL,

      NULL,
      NULL,
      NULL,
      BTreeClass,

      napi_default,
      NULL
    }
  };

  NAPI_CALL(env, napi_define_properties(env, globl, 2, &desc));
}

napi_value init(napi_env env, napi_value exports) {
  napi_property_descriptor desc = {
    "hello",
    NULL,
    __hello,
    NULL,
    NULL,
    NULL,
    napi_default,
    NULL
  };

  NAPI_CALL(env, napi_define_properties(env, exports, 1, &desc));

  initGlobals(env);

  return exports;
}
