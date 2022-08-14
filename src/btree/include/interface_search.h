#include <glib.h>
#include <node_api.h>

typedef GTreeNode* (*glibGtreeNodeStepFunc)(GTreeNode *node);
typedef GTreeNode* (*glibGtreeNodeGetterFunc)(GTree *tree);

napi_value
esFirst(napi_env env, napi_callback_info cbInfo);

napi_value
esLast(napi_env env, napi_callback_info cbInfo);

napi_value
esBefore(napi_env env, napi_callback_info cbInfo);

napi_value
esAfter(napi_env env, napi_callback_info cbInfo);

napi_value
esBetween(napi_env env, napi_callback_info cbInfo);
