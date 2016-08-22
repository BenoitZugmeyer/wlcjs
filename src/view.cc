// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#include "./view.h"
#include "./types.h"

namespace wlcjs {
namespace View {

#define UNWRAP_VIEW \
  wlc_handle view; \
  if (!TryCast(info[0], &view)) { \
    THROW(TypeError, "First argument must be a view"); \
  }

METHOD(Focus) {
  UNWRAP_VIEW
  wlc_view_focus(view);
}

METHOD(Close) {
  UNWRAP_VIEW
  wlc_view_close(view);
}

METHOD(GetOutput) {
  UNWRAP_VIEW
  ISOLATE(info);
  RETURN(Number::New(isolate, wlc_view_get_output(view)));
}

METHOD(SetOutput) {
  UNWRAP_VIEW
  wlc_handle output;
  if (!TryCast(info[1], &output)) {
    THROW(TypeError, "Second argument must be an output");
  }
  wlc_view_set_output(view, output);
}

METHOD(SendToBack) {
  UNWRAP_VIEW
  wlc_view_send_to_back(view);
}

METHOD(SendBelow) {
  UNWRAP_VIEW
  wlc_handle other;
  if (!TryCast(info[1], &other)) {
    THROW(TypeError, "Second argument must be a view");
  }
  wlc_view_send_below(view, other);
}

METHOD(BringAbove) {
  UNWRAP_VIEW
  wlc_handle other;
  if (!TryCast(info[1], &other)) {
    THROW(TypeError, "Second argument must be a view");
  }
  wlc_view_bring_above(view, other);
}

METHOD(BringToFront) {
  UNWRAP_VIEW
  wlc_view_bring_to_front(view);
}

METHOD(GetGeometry) {
  UNWRAP_VIEW

  const wlc_geometry* geometry = wlc_view_get_geometry(view);
  if (!geometry) return;

  Local<Object> result;
  if (!TryCast(geometry, &result)) return;

  RETURN(result);
}

METHOD(SetGeometry) {
  UNWRAP_VIEW

  wlc_geometry geometry;
  uint32_t edge;

  if (!TryCast(info[2], &geometry)) {
    THROW(TypeError, "Third argument is not a valid geometry object");
  }

  if (info[1]->IsUndefined()) {
    edge = 0;
  } else if (!TryCast(info[1], &edge)) {
    THROW(TypeError, "Second argument must be a Number");
  }

  wlc_view_set_geometry(view, edge, &geometry);
}

METHOD(GetState) {
  UNWRAP_VIEW
  ISOLATE(info);
  RETURN(Number::New(isolate, wlc_view_get_state(view)));
}

METHOD(SetState) {
  UNWRAP_VIEW
  uint32_t state;
  bool value;
  if (!TryCast(info[1], &state)) {
    THROW(TypeError, "Second argument is not a Number");
  }
  if (!TryCast(info[2], &value)) {
    THROW(TypeError, "Third argument is not a Boolean");
  }
  wlc_view_set_state(view, static_cast<wlc_view_state_bit>(state), value);
}

METHOD(GetTitle) {
  UNWRAP_VIEW
  RETURN(NewString(wlc_view_get_title(view)));
}


void Export(Local<Object> exports) {
  NODE_SET_METHOD(exports, "focus", Focus);
  NODE_SET_METHOD(exports, "close", Close);
  NODE_SET_METHOD(exports, "getOutput", GetOutput);
  NODE_SET_METHOD(exports, "setOutput", SetOutput);
  NODE_SET_METHOD(exports, "sendToBack", SendToBack);
  NODE_SET_METHOD(exports, "sendBelow", SendBelow);
  NODE_SET_METHOD(exports, "bringAbove", BringAbove);
  NODE_SET_METHOD(exports, "bringToFront", BringToFront);
  NODE_SET_METHOD(exports, "getGeometry", GetGeometry);
  NODE_SET_METHOD(exports, "setGeometry", SetGeometry);
  NODE_SET_METHOD(exports, "getState", GetState);
  NODE_SET_METHOD(exports, "setState", SetState);
  NODE_SET_METHOD(exports, "getTitle", GetTitle);
}

}  // namespace View
}  // namespace wlcjs
