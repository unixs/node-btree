#include <node_api.h>

typedef struct {
  napi_value esbTree;
  napi_value callback;
  napi_value cbThis;
  size_t idx;
  BTree_t *bTree;
  gpointer data;
} ForEachContext_t;

typedef void (*forEachCallback)(ForEachContext_t *ctxt);
