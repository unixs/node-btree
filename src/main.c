#include <stdio.h>
#include <node_api.h>
#include "include/common.h"

napi_value init(napi_env env, napi_value exports);

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

  napi_value instance;

  NAPI_CALL(env, napi_create_object(env, &instance));

  return instance;
}

void initGlobals(napi_env env) {
  napi_value globl;
  NAPI_CALL(env, napi_get_global(env, &globl));

  napi_value fake;
  NAPI_CALL(env, napi_create_int64(env, 100500, &fake));

  napi_value BTreeClass;

  NAPI_CALL(env, napi_define_class(env, "BTree", NAPI_AUTO_LENGTH, BTreeConstructor, NULL, 0, NULL, &BTreeClass));

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
