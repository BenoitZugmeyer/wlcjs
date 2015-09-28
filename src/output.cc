#include "types.h"

namespace wlcjs {
namespace Output {

#define UNWRAP_OUTPUT \
  wlc_handle output; \
  if (!TryCast(info[0], &output)) THROW(TypeError, "First argument must be an output");

METHOD(GetName) {
  UNWRAP_OUTPUT
  RETURN(info, NewString(wlc_output_get_name(output)));
}

METHOD(GetViews) {
  ISOLATE(info);
  UNWRAP_OUTPUT

  size_t memb;
  const wlc_handle* views = wlc_output_get_views(output, &memb);
  Local<Array> result = Array::New(isolate, memb);
  Local<Context> context = isolate->GetCurrentContext();

  for (size_t i = 0; i < memb; i += 1) {
    if (result->Set(context, i, Number::New(isolate, views[i])).IsNothing()) {
      return;
    }
  }

  RETURN(info, result);
}

METHOD(GetResolution) {
  UNWRAP_OUTPUT

  const wlc_size* resolution = wlc_output_get_resolution(output);
  if (!resolution) return;

  Local<Object> resolution_js;
  if (!TryCast(resolution, &resolution_js)) return;

  RETURN(info, resolution_js);
}

void Export(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getName", GetName);
  NODE_SET_METHOD(exports, "getViews", GetViews);
  NODE_SET_METHOD(exports, "getResolution", GetResolution);
}

}
}
