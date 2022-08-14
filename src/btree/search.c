#include <core.h>
#include <interface.h>

#include <inline_core.h>

static char *msgKeyNotFound = "key not found";

static napi_value
esEdge(napi_env env, napi_callback_info cbInfo, glibGtreeNodeGetterFunc getterFunc) {
  napi_value esThis;
  size_t argc = 0;

  NAPI_CALL(env, false, napi_get_cb_info(env, cbInfo, &argc, NULL, &esThis, NULL));

  BTree_t *btree;
  EXTRACT_BTREE(env, esThis, btree);

  GTreeNode *node = getterFunc(btree->nativeTree);

  if (node) {
    return cloneInternalEsObject(env, getEsGTreeNode(node)->esKeyValue);
  }
  else {
    return getEsUndef(env);
  }
}

napi_value
esFirst(napi_env env, napi_callback_info cbInfo) {
  return esEdge(env, cbInfo, g_tree_node_first);
}

napi_value
esLast(napi_env env, napi_callback_info cbInfo) {
  return esEdge(env, cbInfo, g_tree_node_last);
}

napi_value
esSearch(napi_env env, napi_callback_info cbInfo, glibGtreeNodeStepFunc stepFunc) {
  napi_value esThis, argv[2], searchKey, esSkipFirst;
  size_t argc = 2;
  bool skipFirst = FALSE;

  NAPI_CALL(env, false, napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));
  CHECK_ARGC(1, msgTooFewArguments);

  searchKey = argv[0];

  if (argc > 1) {
    esSkipFirst = argv[1];
    NAPI_CALL(env, false,
      napi_coerce_to_bool(env, esSkipFirst, &esSkipFirst));
    NAPI_CALL(env, false,
      napi_get_value_bool(env, esSkipFirst, &skipFirst));
  }

  BTree_t *btree;
  EXTRACT_BTREE(env, esThis, btree);

  BTree_t *newBtree;
  napi_value newEsTree = cloneBtreeWrapper(env, esThis, &newBtree);

  GTreeNode *current = btreeLookup(env, btree, searchKey);

  if (!current) {
    NAPI_CALL(env, false,
      napi_throw_error(env, NULL, msgKeyNotFound));

      return NULL;
  }

  if (skipFirst) {
    current = stepFunc(current);
  }

  BTreeNode newNode;

  while (current) {
    newNode = cloneBTreeNode(env, newBtree, current);

    g_tree_replace(newBtree->nativeTree, newNode, newNode);

    current = stepFunc(current);
  }

  return newEsTree;
}

napi_value
esBefore(napi_env env, napi_callback_info cbInfo) {
  return esSearch(env, cbInfo, g_tree_node_previous);
}

napi_value
esAfter(napi_env env, napi_callback_info cbInfo) {
  return esSearch(env, cbInfo, g_tree_node_next);
}

napi_value
esBetween(napi_env env, napi_callback_info cbInfo) {
  napi_value esThis, argv[3], lowerSearchKey, upperSearchKey, esSkipEdges;
  size_t argc = 3;
  bool skipEdges = FALSE;

  NAPI_CALL(env, false, napi_get_cb_info(env, cbInfo, &argc, argv, &esThis, NULL));
  CHECK_ARGC(2, msgTooFewArguments);

  lowerSearchKey = argv[0];
  upperSearchKey = argv[1];

  if (argc > 2) {
    esSkipEdges = argv[2];
    NAPI_CALL(env, false,
      napi_coerce_to_bool(env, esSkipEdges, &esSkipEdges));
    NAPI_CALL(env, false,
      napi_get_value_bool(env, esSkipEdges, &skipEdges));
  }

  BTree_t *btree;
  EXTRACT_BTREE(env, esThis, btree);

  GTreeNode *lowerGNode = btreeLookup(env, btree, lowerSearchKey);
  if (!lowerGNode) {
    NAPI_CALL(env, false,
      napi_throw_error(env, NULL, "lower key not found in tree"));

      return NULL;
  }

  GTreeNode *upperGNode = btreeLookup(env, btree, upperSearchKey);
  if (!upperGNode) {
    NAPI_CALL(env, false,
      napi_throw_error(env, NULL, "upper key not found in tree"));

      return NULL;
  }

  gint compareResult = compareGTreeNodes(lowerGNode, upperGNode, btree);

  BTree_t *newBtree;
  BTreeNode newNode;
  napi_value newEsTree = cloneBtreeWrapper(env, esThis, &newBtree);

  // lower key == upper key
  if (compareResult == 0) {
    if (!skipEdges) {
      newNode = cloneBTreeNode(env, newBtree, lowerGNode);
      g_tree_replace(newBtree->nativeTree, newNode, newNode);
    }

    return newEsTree;
  }

  // lower key > upper key
  if (compareResult > 0) {
    GTreeNode *tmp = lowerGNode;
    lowerGNode = upperGNode;
    upperGNode = tmp;
  }

  // lower key < upper key
  GTreeNode *current = lowerGNode;
  GTreeNode *last = upperGNode;

  if (skipEdges) {
    current = g_tree_node_next(lowerGNode);

    if (current == last) {
      return newEsTree; // return empty tree
    }
  }


  while (current != last) {
    newNode = cloneBTreeNode(env, newBtree, current);

    g_tree_replace(newBtree->nativeTree, newNode, newNode);

    current = g_tree_node_next(current);
  }

  if (!skipEdges) {
    newNode = cloneBTreeNode(env, newBtree, last);
    g_tree_replace(newBtree->nativeTree, newNode, newNode);
  }

  return newEsTree;
}
