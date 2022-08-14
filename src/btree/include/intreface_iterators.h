#include <node_api.h>
#include <iterators.h>


void
iteratorResultDefaultCb(IteratorContext_t *ctxt);
void
iteratorResultValueCb(IteratorContext_t *ctxt);
void
iteratorResultKeyCb(IteratorContext_t *ctxt);

napi_value
esGenerator(napi_env env, napi_callback_info cbInfo);
