#include "types.h"

namespace wlcjs {
namespace View {

#define UNWRAP_VIEW \
  wlc_handle view; \
  if (!TryCast(info[0], &view)) THROW(TypeError, "First argument must be a view");

METHOD(GetTitle) {
  UNWRAP_VIEW
  RETURN(info, NewString(wlc_view_get_title(view)));
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
  RETURN(info, Number::New(isolate, wlc_view_get_output(view)));
}

METHOD(SetOutput) {
  UNWRAP_VIEW
  wlc_handle output;
  if (!TryCast(info[1], &output)) THROW(TypeError, "Second argument must be an output");
  wlc_view_set_output(view, output);
}

METHOD(SendToBack) {
  UNWRAP_VIEW
  wlc_view_send_to_back(view);
}

METHOD(BringToFront) {
  UNWRAP_VIEW
  wlc_view_bring_to_front(view);
}

METHOD(SetGeometry) {
  UNWRAP_VIEW

  wlc_geometry geometry;
  uint32_t edge;

  if (!TryCast(info[2], &geometry)) THROW(TypeError, "Third argument is not a valid geometry object");

  if (info[1]->IsUndefined()) {
    edge = 0;
  }
  else if (!TryCast(info[1], &edge)) {
    THROW(TypeError, "Second argument must be a Number");
  }

  wlc_view_set_geometry(view, edge, &geometry);
}

void Export(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getTitle", GetTitle);
  NODE_SET_METHOD(exports, "focus", Focus);
  NODE_SET_METHOD(exports, "close", Close);
  NODE_SET_METHOD(exports, "getOutput", GetOutput);
  NODE_SET_METHOD(exports, "setOutput", SetOutput);
  NODE_SET_METHOD(exports, "sendToBack", SendToBack);
  NODE_SET_METHOD(exports, "bringToFront", BringToFront);
  NODE_SET_METHOD(exports, "setGeometry", SetGeometry);
}

}
}
