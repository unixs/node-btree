#include <glib.h>

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
  GCompareDataFunc key_compare;
  GDestroyNotify key_destroy_func;
  GDestroyNotify value_destroy_func;
  gpointer key_compare_data;
  guint nnodes;
  gint ref_count;
};

static inline GTreeNode *local_g_tree_first_node(GTree *tree) {
  GTreeNode *tmp;

  if (!tree->root)
    return NULL;

  tmp = tree->root;

  while (tmp->left_child)
    tmp = tmp->left;

  return tmp;
}

static inline GTreeNode *local_g_tree_node_next(GTreeNode *node) {
  GTreeNode *tmp;

  tmp = node->right;

  if (node->right_child)
    while (tmp->left_child)
      tmp = tmp->left;

  return tmp;
}

// / FROM glib/gtree.c
