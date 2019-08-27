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

napi_value init(napi_env env, napi_value exports) {
  napi_status status;

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

  status = napi_define_properties(env, exports, 1, &desc);

  if (status != napi_ok) {
    return NULL;
  }

  return exports;
}
