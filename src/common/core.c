#include <core.h>

const char *msgTooFewArguments = "Too few arguments.";
const char *msgCorrupt = "BTree corrupt. Next node expected but it is null.";

void
nativeInsertNode(napi_env env, napi_value esBtree, napi_value box)
{
  BTree_t *bTree;

  // NOTE: Memory policy: New es and  create new native wrapper

  // Extract native BTree pointer
  NAPI_CALL(env, false, napi_unwrap(env, esBtree, (void **) &bTree));

  // Set ref counter to 1 for protect from GC
  napi_ref nodeRef;
  NAPI_CALL(env, false, napi_create_reference(env, box, 1, &nodeRef));

  // Alloc new tree node
  BTreeNode node;
  NEW_NODE(node, bTree, nodeRef);

  // Add es plain object to native bTree
  g_tree_replace(bTree->nativeTree, node, node);
}

napi_value
getEsUndef(napi_env env)
{
  napi_value esUndef;
  NAPI_CALL(env, false, napi_get_undefined(env, &esUndef));

  return esUndef;
}

napi_value
getEsNull(napi_env env)
{
  napi_value esNull;
  NAPI_CALL(env, false, napi_get_null(env, &esNull));

  return esNull;
}

napi_value
getEsGlobal(napi_env env)
{
  napi_value result;
  NAPI_CALL(env, true, napi_get_global(env, &result));

  return result;
}

napi_value
unrefBtreeNodeEsObject(napi_env env, napi_ref ref, napi_value *key,
                       napi_value *value)
{
  napi_value result;

  NAPI_CALL(env, false, napi_get_reference_value(env, ref, &result));

  if (key) {
    NAPI_CALL(env, false, napi_get_named_property(env, result, KEY, key));
  }

  if (value) {
    NAPI_CALL(env, false, napi_get_named_property(env, result, VALUE, value));
  }

  return result;
}

static gint
to_ptr_array_cb(gpointer a, gpointer b, gpointer data)
{
  GPtrArray *buff = (GPtrArray *) data;

  BTreeNode node = (BTreeNode) a;

  g_ptr_array_add(buff, node->esKeyValue); // add napi_ref from key field

  return FALSE;
}


GPtrArray *
gtreeToPtrArray(GTree *gtree)
{
  gint tree_size = g_tree_nnodes(gtree);

  GPtrArray *buff = g_ptr_array_sized_new(tree_size);

  g_tree_foreach(gtree, to_ptr_array_cb, (gpointer) buff);

  return buff;
}

// TODO: possible inline
napi_ref
cloneInternalEsRef(napi_env env, napi_ref ref)
{
  napi_ref result;

  napi_value box = cloneInternalEsObject(env, ref);

  NAPI_CALL(env, false, napi_create_reference(env, box, 1, &result));

  return result;
}

#ifdef HAS_GTREE_NODE

napi_value
getNodeEsObject(napi_env env, GTreeNode *g_node)
{
  if (g_node) {
    napi_value result;
    BTreeNode node = (BTreeNode) g_tree_node_value(g_node);

    NAPI_CALL(env, false,
              napi_get_reference_value(env, node->esKeyValue, &result));

    return result;
  }
  else {
    return getEsUndef(env);
  }
}

napi_value
getNodeEsField(napi_env env, GTreeNode *g_node, const char *field)
{
  if (g_node) {
    napi_valuetype resType;
    napi_value result = getNodeEsObject(env, g_node);

    NAPI_CALL(env, false, napi_typeof(env, result, &resType));

    if (resType != napi_undefined) {
      NAPI_CALL(env, false,
                napi_get_named_property(env, result, field, &result));

      return result;
    }
    else {
      return result;
    }
  }
  else {
    return getEsUndef(env);
  }
}

napi_value
getNodeEsFieldPair(napi_env env, GTreeNode *g_node)
{
  if (g_node) {
    napi_value array;

    NAPI_CALL(env, false, napi_create_array_with_length(env, 2, &array));

    NAPI_CALL(env, false,
              napi_set_element(env, array, 0, getNodeEsKey(env, g_node)));

    NAPI_CALL(env, false,
              napi_set_element(env, array, 1, getNodeEsValue(env, g_node)));

    return array;
  }
  else {
    return getEsUndef(env);
  }
}

GTreeNode *
btreeLookup(napi_env env, BTree_t *btree, napi_value key)
{
  napi_value lookupBox;
  napi_ref lookupRef;

  NAPI_CALL(env, false, napi_create_object(env, &lookupBox));

  NAPI_CALL(env, false, napi_set_named_property(env, lookupBox, KEY, key));

  NAPI_CALL(env, false, napi_create_reference(env, lookupBox, 0, &lookupRef));

  BTreeNode_t lookupNode = {lookupRef, NULL};

  return g_tree_lookup_node(btree->nativeTree, &lookupNode);
}

napi_value
cloneBtreeWrapper(napi_env env, napi_value orig, BTree_t **newBTreePtr)
{
  extern napi_ref btreeConstructorRef;
  napi_value result, comparator, constructor;
  BTree_t *origBTree;

  // Extract native BTree pointer
  EXTRACT_BTREE(env, orig, origBTree);

  NAPI_CALL(env, true,
            napi_get_reference_value(env, btreeConstructorRef, &constructor));

  NAPI_CALL(env, true,
            napi_get_reference_value(env, origBTree->comparator, &comparator));

  // TODO: Check constructor ref. Can we increment counter only?
  NAPI_CALL(env, true,
            napi_new_instance(env, constructor, 1, &comparator, &result));

  // Pass btree ptr in out
  if (newBTreePtr) {
    EXTRACT_BTREE(env, result, (*newBTreePtr));
  }

  return result;
}

napi_value
cloneInternalEsObject(napi_env env, napi_ref ref)
{
  napi_value result, key, value;

  NAPI_CALL(env, false, napi_create_object(env, &result));

  unrefBtreeNodeEsObject(env, ref, &key, &value);

  // Set key & value to box
  NAPI_CALL(env, false, napi_set_named_property(env, result, KEY, key));
  NAPI_CALL(env, false, napi_set_named_property(env, result, VALUE, value));

  return result;
}

// TODO: possible inline
BTreeNode
cloneBTreeNode(napi_env env, BTree_t *btree, GTreeNode *node)
{
  napi_ref origEsNodeRef = getEsGTreeNode(node)->esKeyValue;
  napi_ref newEsNodeRef = cloneInternalEsRef(env, origEsNodeRef);

  BTreeNode result;
  NEW_NODE(result, btree, newEsNodeRef);

  return result;
}

#endif // HAS_GTREE_NODE
