// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#include "./types.h"

namespace wlcjs {
namespace Output {

#define UNWRAP_OUTPUT \
  wlc_handle output; \
  if (!TryCast(info[0], &output)) { \
    THROW(TypeError, "First argument must be an output"); \
  }

METHOD(GetName) {
  UNWRAP_OUTPUT
  RETURN(NewString(wlc_output_get_name(output)));
}

METHOD(GetViews) {
  UNWRAP_OUTPUT

  size_t memb;
  Local<Array> result;

  const wlc_handle* views = wlc_output_get_views(output, &memb);
  if (!TryCast(views, memb, &result)) return;

  RETURN(result);
}

METHOD(GetResolution) {
  UNWRAP_OUTPUT

  const wlc_size* resolution = wlc_output_get_resolution(output);
  if (!resolution) return;

  Local<Object> resolution_js;
  if (!TryCast(resolution, &resolution_js)) return;

  RETURN(resolution_js);
}

void Export(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getName", GetName);
  NODE_SET_METHOD(exports, "getViews", GetViews);
  NODE_SET_METHOD(exports, "getResolution", GetResolution);
}

}  // namespace Output
}  // namespace wlcjs
