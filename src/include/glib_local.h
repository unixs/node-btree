#include <glib.h>

typedef struct __FakeNode* GTreeNode_t;

// FROM glib/gtree.c

typedef struct _GTreeNode GTreeNode;

struct _GTreeNode {
  gpointer key;     /* key for this node */
  gpointer value;   /* value stored at this node */
  GTreeNode *left;  /* left subtree */
  GTreeNode *right; /* right subtree */
  gint8 balance;    /* height (right) - height (left) */
  guint8 left_child;
  guint8 right_child;
};

struct _GTree {
  GTreeNode *root;
};

#define local_g_tree_node_value(node) ((GTreeNode *) node)->value

static inline GTreeNode_t local_g_tree_first_node(GTree *tree) {
  GTreeNode *tmp;

  if (!tree->root)
    return NULL;

  tmp = tree->root;

  while (tmp->left_child)
    tmp = tmp->left;

  return (GTreeNode_t) tmp;
}

static inline GTreeNode_t local_g_tree_node_next(GTreeNode_t fake) {
  GTreeNode *node = (GTreeNode *) fake;
  GTreeNode *tmp;

  tmp = node->right;

  if (node->right_child)
    while (tmp->left_child)
      tmp = tmp->left;

  return (GTreeNode_t) tmp;
}

// / FROM glib/gtree.c
