#define NAPI_CALL(env, call)                                                   \
  do {                                                                         \
    napi_status status = (call);                                               \
    if (status != napi_ok) {                                                   \
      const napi_extended_error_info *error_info = NULL;                       \
      napi_get_last_error_info((env), &error_info);                            \
      bool is_pending;                                                         \
      napi_is_exception_pending((env), &is_pending);                           \
      if (!is_pending) {                                                       \
        const char *message = (error_info->error_message == NULL)              \
                                  ? "empty error message"                      \
                                  : error_info->error_message;                 \
        napi_throw_error((env), NULL, message);                                \
        return (napi_value) NULL;                                              \
      }                                                                        \
    }                                                                          \
  } while (0)

/**
 * Get globas ES Symbol
 */
#define NAPI_GLOBAL_SYM(env, name, napiVar)                                    \
{                                                                              \
  napi_value global, Symbol;                                                   \
  NAPI_CALL(env, napi_get_global(env, &global));                               \
  NAPI_CALL(env, napi_get_named_property(env, global, "Symbol", &Symbol));     \
  NAPI_CALL(env, napi_get_named_property(env, Symbol, name, &napiVar));        \
}

/**
 * Allocate memory for & initialize new node
 */
#define NEW_NODE(ptr, bTree, ref)                                              \
ptr = g_new(BTreeNode_t, 1);                                                   \
ptr->bTree = bTree;                                                            \
ptr->esKeyValue = ref;                                                         \

/**
 * Free node
 */
#define FREE_NODE(node)                                                        \
NAPI_CALL(((BTreeNode) node)->bTree->env,                                      \
  napi_delete_reference(((BTreeNode) node)->bTree->env,                        \
    ((BTreeNode) node)->esKeyValue));                                          \
g_free((gpointer) node);
