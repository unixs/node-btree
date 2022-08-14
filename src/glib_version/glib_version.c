#include <stdio.h>
#include <glib.h>

#include <utils.h>
#include <export.h>
#include <glib_version.h>

napi_ref glibVersionRef;

static napi_value
esGlibVersionFull(napi_env env, napi_callback_info cbInfo) {
  napi_value esFull;

  char fullVersion[20] = "";

  size_t versionStrSize = snprintf(
    fullVersion,
    sizeof(fullVersion),
    "%u.%u.%u",
    GLIB_MAJOR_VERSION,
    GLIB_MINOR_VERSION,
    GLIB_MICRO_VERSION
  );

  NAPI_CALL(env, false,
    napi_create_string_utf8(env, fullVersion, versionStrSize, &esFull));

  return esFull;
}

static napi_value
esGlibVersionMajor(napi_env env, napi_callback_info cbInfo) {
  napi_value esMajor;

  NAPI_CALL(env, false,
    napi_create_int64(env, GLIB_MAJOR_VERSION, &esMajor));

  return esMajor;
}

static napi_value
esGlibVersionMinor(napi_env env, napi_callback_info cbInfo) {
  napi_value esMinor;

  NAPI_CALL(env, false,
    napi_create_int64(env, GLIB_MINOR_VERSION, &esMinor));

  return esMinor;
}

static napi_value
esGlibVersionPatch(napi_env env, napi_callback_info cbInfo) {
  napi_value esPatch;

  NAPI_CALL(env, false,
    napi_create_int64(env, GLIB_MICRO_VERSION, &esPatch));

  return esPatch;
}

static napi_value
esHasGTreeNode(napi_env env, napi_callback_info cbInfo) {
  napi_value result;

  #ifdef HAS_GTREE_NODE
    NAPI_CALL(env, false, napi_get_boolean(env, true, &result));
  #else
    NAPI_CALL(env, false, napi_get_boolean(env, false, &result));
  #endif

  return result;
}

napi_property_descriptor
glib_version_export(napi_env env) {
  napi_value esGlibVersion;

  NAPI_CALL(env, false, napi_create_object(env, &esGlibVersion));

  // Instance props
  const napi_property_descriptor esGlibVersionProps[] = {
    DEF_PROP("FULL", esGlibVersionFull),
    DEF_PROP("MAJOR", esGlibVersionMajor),
    DEF_PROP("MINOR", esGlibVersionMinor),
    DEF_PROP("PATCH", esGlibVersionPatch),
    DEF_METHOD("hasGTreeNode", esHasGTreeNode)
  };

  NAPI_CALL(env, false,
    napi_define_properties(
      env,
      esGlibVersion,
      (sizeof(esGlibVersionProps) / sizeof(napi_property_descriptor)),
      esGlibVersionProps
    )
  );

  NAPI_CALL(env, false, napi_object_freeze(env, esGlibVersion));

  NAPI_CALL(env, false,
    napi_create_reference(env, esGlibVersion, 1, &glibVersionRef));

  napi_property_descriptor descriptor = {
    "GLIB_VERSION",
    NULL,
    NULL,
    NULL,
    NULL,
    esGlibVersion,
    napi_default,
    NULL
  };

  return descriptor;
}
