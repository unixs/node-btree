#include <core.h>
#include <comparator.h>


/**
 * Free allocated GTree & unref comparator for GC access
 */
void
freeNativeBTree(napi_env env, void *finalize_data, void *finalize_hint) {
  BTree_t *bTree = (BTree_t *) finalize_data;

  // Unref comparator function for GC access
  NAPI_CALL(env, false,
    napi_delete_reference(env, bTree->comparator));
  // Destroy native bTree & release memory
  g_tree_destroy(bTree->nativeTree);

  // Release BTree_t struct memory
  g_free((gpointer) bTree);
}

static void
freeTreeValue(gpointer treeValue) {
  FREE_NODE(treeValue);
}

BTree_t*
btreeAlloc(napi_env env)
{
    // Allocate memory for usre data wich recived in native comparator
    BTree_t *bTree = g_new(BTree_t, 1);

    // Initialize native BTree with native comparator & additional user data
    // Key & Value is same pointer (ES object). Need free one of key or value.
    GTree *nativeTree = g_tree_new_full(nativeComparator, bTree, NULL, freeTreeValue);

    // Fill user data
    bTree->nativeTree = nativeTree;
    bTree->env = env;

    return bTree;
}
