#include <export.h>
#include <utils.h>

/**
 * Module initialization callback
 */
static napi_value
init(napi_env env, napi_value exports) {
  napi_property_descriptor props[] = {
    btree_export(env),
    glib_version_export(env)
  };

  NAPI_CALL(env, false,
    napi_define_properties(
      env,
      exports,
      (sizeof(props) / sizeof(napi_property_descriptor)),
      props
    )
  );

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);
