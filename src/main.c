#include <stdio.h>
#include <stdlib.h>
#include <node_api.h>
#include "include/common.h"
#include <glib.h>

typedef struct {
  napi_env env;
  napi_value comparator;
  GTree *nativeTree;
} BTree_t;

gint nativeComparator(gconstpointer a, gconstpointer b, gpointer bTree) {
  napi_value strA, strB;

  BTree_t *bTreeWrap = (BTree_t *) bTree;
  napi_env env = bTreeWrap->env;

  NAPI_CALL(env, napi_coerce_to_string(env, a, &strA));
  NAPI_CALL(env, napi_coerce_to_string(env, b, &strB));

  char strBufA[100] = "\0";
  char strBufB[100] = "\0";

  NAPI_CALL(env, napi_get_value_string_utf8(env, strA, strBufA, 100, NULL));
  NAPI_CALL(env, napi_get_value_string_utf8(env, strB, strBufB, 100, NULL));

  printf("%s\n", strBufA);
  printf("%s\n", strBufB);

  napi_value esNull;
  napi_value argv[] = {a, b};
  napi_value esCompareResult;
  NAPI_CALL(env, napi_get_null(env, &esNull));
  NAPI_CALL(env, napi_call_function(env, esNull, bTreeWrap->comparator, 2, argv, &esCompareResult));

  int64_t compareResult;

  NAPI_CALL(env, napi_get_value_int64(env, esCompareResult, &compareResult));

  return compareResult;
}

napi_value esBTreeHeight(napi_env env, napi_callback_info cbInfo) {
  napi_value esHeight;
  napi_value esThis;
  BTree_t *bTree;

  // Get es this
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  // Native call to glib tree
  gint nativeHeight = g_tree_height(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env, napi_create_int64(env, nativeHeight, &esHeight));

  return esHeight;
}

napi_value esBTreeSize(napi_env env, napi_callback_info cbInfo) {
  napi_value esSize;
  napi_value esThis;
  BTree_t *bTree;

  // Get es this
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, NULL, NULL, &esThis, NULL));

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  // Native call to glib tree
  gint nativeSize = g_tree_nnodes(bTree->nativeTree);

  // Convert from C type to es type
  NAPI_CALL(env, napi_create_int64(env, nativeSize, &esSize));

  return esSize;
}

napi_value esBTreeSet(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  BTree_t *bTree;
  size_t argc = 2;
  napi_value argv[2];

  // Get es this
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 2) {
    NAPI_CALL(env, napi_throw_error(env, NULL, "Expected two arguments."));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  // Native call to glib tree
  g_tree_insert(bTree->nativeTree, argv[0], argv[1]);

  return esThis;
}

napi_value esBTreeGet(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis;
  napi_value result;
  BTree_t *bTree;
  size_t argc = 1;
  napi_value argv[argc];

  napi_value strKey, strVal;

  char strBufKey[100] = "\0";
  char strBufVal[100] = "\0";


  // Get es this
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));

  if (argc < 1) {
    NAPI_CALL(env, napi_throw_error(env, "10", "Expected one argument."));
  }

  // Extract native BTree pointer
  NAPI_CALL(env, napi_unwrap(env, esThis, &bTree));

  NAPI_CALL(env, napi_coerce_to_string(env, argv[0], &strKey));
  NAPI_CALL(env, napi_get_value_string_utf8(env, strKey, strBufKey, 100, NULL));
  printf("key: %s\n", strBufKey);

  // Native call to glib tree
  napi_value value = g_tree_lookup(bTree->nativeTree, argv[0]);

  napi_valuetype type;

  do {
    napi_status status = (napi_typeof(env, value, &type));
    if (status != napi_ok) {
      const napi_extended_error_info *error_info = NULL;
      napi_get_last_error_info((env), &error_info);
      bool is_pending;
      napi_is_exception_pending((env), &is_pending);
      if (!is_pending) {
        const char *message = (error_info->error_message == NULL)
                                  ? "empty error message"
                                  : error_info->error_message;
        napi_throw_error((env), NULL, message);
        return NULL;
      }
    }
  } while (0);

  NAPI_CALL(env, napi_coerce_to_string(env, value, &strVal));
  NAPI_CALL(env, napi_get_value_string_utf8(env, strVal, strBufVal, 100, NULL));
  printf("val: %s\n", strBufVal);

  if (value == NULL) {
    NAPI_CALL(env, napi_get_null(env, &result));
  }
  else {
    result = value;
  }

  return result;
}

void freeBTree(napi_env env, void *finalize_data, void *finalize_hint) {
  // BUG: Incorrect memory free.
  free(finalize_data);
}

napi_value init(napi_env env, napi_value exports);

napi_ref constructor;

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);

napi_value __hello(napi_env env, napi_callback_info info) {
  napi_value dbl, str;

  size_t argc = 1;
  napi_value argv[1], esThis;
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &esThis, NULL));

  NAPI_CALL(env, napi_create_string_utf8(env, "bar", NAPI_AUTO_LENGTH, &str));
  NAPI_CALL(env, napi_create_double(env, 10.5, &dbl));

  NAPI_CALL(env, napi_set_named_property(env, argv[0], "foo", str));

  return dbl;
}

napi_value BTreeConstructor(napi_env env, napi_callback_info cbInfo) {
  printf("Constructor called.\n");

  napi_value esBtree;
  napi_ref ref;

  size_t argc = 1;
  napi_value args[1];
  NAPI_CALL(env, napi_get_cb_info(env, cbInfo, &argc, args, &esBtree, NULL));

  // Allocate memory for usre data wich recived in native comparator
  BTree_t *bTree = (BTree_t *) malloc(sizeof(BTree_t));

  // Initialize native BTree with native comparator & additional user data
  GTree *nativeTree = g_tree_new_with_data(nativeComparator, bTree);

  // Fill user data
  bTree->nativeTree = nativeTree;
  bTree->comparator = args[0];
  bTree->env = env;


  napi_valuetype comparatorType;
  NAPI_CALL(env, napi_typeof(env, bTree->comparator, &comparatorType));

  if (comparatorType != napi_function) {
    NAPI_CALL(env, napi_throw_error(env, "10", "First arg must be comparator function"));
  }

  // Create es function
  //napi_value esBTreeSetCallback;
  //NAPI_CALL(env, napi_create_function(env, "set", NAPI_AUTO_LENGTH, esBTreeSet, NULL, &esBTreeSetCallback));

  // Define comparator as not enumerable & ro property of es btree instance
  napi_property_descriptor esBTreeProps[] = {{
    "comparator",
    NULL,

    NULL,
    NULL,
    NULL,
    bTree->comparator,

    napi_default,
    NULL
  }, {
    "height",
    NULL,

    NULL,
    esBTreeHeight,
    NULL,
    NULL,

    napi_default,
    NULL
  }, {
    "size",
    NULL,

    esBTreeSize,
    NULL,
    NULL,
    NULL,

    napi_default,
    NULL
  }, {
    "set",
    NULL,

    esBTreeSet,
    NULL,
    NULL,
    NULL,

    napi_default,
    NULL
  }, {
    "get",
    NULL,

    esBTreeGet,
    NULL,
    NULL,
    NULL,

    napi_default,
    NULL
  }};

  size_t propsCnt = sizeof(esBTreeProps) / sizeof(esBTreeProps[0]);
  NAPI_CALL(env, napi_define_properties(env, esBtree, propsCnt, esBTreeProps));

  // Wrap native data in ES variable for native access again
  NAPI_CALL(env, napi_wrap(env, esBtree, bTree, freeBTree, NULL, &ref));

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
