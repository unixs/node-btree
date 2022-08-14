#include <core.h>
#include <interface.h>
#include <type.h>

/**
 * ES callback. Constructor
 */
napi_value
esConstructor(napi_env env, napi_callback_info cbInfo) {
  napi_value esBtree, comparator, isConstructor;

  size_t argc = 1;
  napi_value argv[1];

  // Get es arguments & context
  NAPI_CALL(env, false,
    napi_get_cb_info(env, cbInfo, &argc, argv, &esBtree, NULL));

  CHECK_ARGC(1, msgTooFewArguments);
  comparator = argv[0];

  NAPI_CALL(env, true,
    napi_get_new_target(env, cbInfo, &isConstructor));

  if (isConstructor) {
    // Check type of first argument. Must be function
    napi_valuetype comparatorType;
    NAPI_CALL(env, false,
      napi_typeof(env, comparator, &comparatorType));

    if (comparatorType != napi_function) {
      NAPI_CALL(env, false,
        napi_throw_error(env, NULL, "First arg must be comparator qsort() like function"));

        return NULL;
    }

    BTree_t *bTree = btreeAlloc(env);

    // Create ref on comparator function. Protect from GC
    NAPI_CALL(env, false,
      napi_create_reference(env, argv[0], 1, &bTree->comparator));

    // Wrap native data in ES variable for native access again
    NAPI_CALL(env, false,
      napi_wrap(env, esBtree, bTree, freeNativeBTree, NULL, NULL));
  }
  else {
    napi_value constructorFunc;

    NAPI_CALL(env, true,
      napi_get_reference_value(env, btreeConstructorRef, &constructorFunc));

    NAPI_CALL(env, true,
      napi_new_instance(env, constructorFunc, argc, argv, &esBtree));
  }

  return esBtree;
}
