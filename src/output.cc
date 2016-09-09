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

METHOD(GetResolution) {
  UNWRAP_OUTPUT

  const wlc_size* resolution = wlc_output_get_resolution(output);
  if (!resolution) return;

  Local<Object> resolution_js;
  if (!TryCast(resolution, &resolution_js)) return;

  RETURN(resolution_js);
}

METHOD(GetMask) {
  UNWRAP_OUTPUT

  const uint32_t mask = wlc_output_get_mask(output);

  Local<Integer> mask_js;
  if (!TryCast(mask, &mask_js)) return;

  RETURN(mask_js);
}

METHOD(SetMask) {
  UNWRAP_OUTPUT

  uint32_t mask;
  if (!TryCast(info[1], &mask)) {
    THROW(TypeError, "Second argument must be a Uint32");
  }

  wlc_output_set_mask(output, mask);
}

METHOD(GetViews) {
  UNWRAP_OUTPUT

  size_t memb;
  Local<Array> result;

  const wlc_handle* views = wlc_output_get_views(output, &memb);
  if (!TryCast(views, memb, &result)) return;

  RETURN(result);
}

void Export(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getName", GetName);
  NODE_SET_METHOD(exports, "getResolution", GetResolution);
  NODE_SET_METHOD(exports, "getMask", GetMask);
  NODE_SET_METHOD(exports, "setMask", SetMask);
  NODE_SET_METHOD(exports, "getViews", GetViews);
}

}  // namespace Output
}  // namespace wlcjs
