#include <core.h>


static void
freeTreeValue(gpointer treeValue)
{
  FREE_NODE(treeValue);
}

static GTree *
newGTree(BTree_t *bTree)
{
  return g_tree_new_full(nativeComparator, bTree, NULL, freeTreeValue);
}

void
reallocGTree(BTree_t *bTree)
{
  g_tree_destroy(bTree->nativeTree);

  bTree->nativeTree = newGTree(bTree);
}

/**
 * Free allocated wrapper, GTree & unref comparator for GC access
 */
void
freeNativeBTree(napi_env env, void *finalize_data, void *finalize_hint)
{
  BTree_t *bTree = (BTree_t *) finalize_data;

  // Unref comparator function for GC access
  NAPI_CALL(env, false, napi_delete_reference(env, bTree->comparator));

  // Destroy all key/value's and release GTree memory
  // https://github.com/GNOME/glib/blob/2.21.6/glib/gtree.c#L322
  g_tree_destroy(bTree->nativeTree);

  // Release BTree_t struct memory
  g_free((gpointer) bTree);
}

BTree_t *
allocNativeBTree(napi_env env)
{
  // Allocate memory for our wrapper
  BTree_t *bTree = g_new(BTree_t, 1);

  // Initialize native BTree with native comparator & additional user data
  // Key & Value is same pointer (ES object). Need free one of key or value.
  GTree *nativeTree = newGTree(bTree);

  // Fill wrapper data
  bTree->nativeTree = nativeTree;
  bTree->env = env;

  return bTree;
}
