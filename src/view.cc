#include "view.h"
#include "output.h"
#include "types.h"

namespace wlcjs {

#define UNWRAP_VIEW \
  View* view; \
  if (!Unwrap(View::FromLocalObject(info.This()), &view)) return;

void ViewGetTitle(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  UNWRAP_VIEW
  RETURN(info, NewString(wlc_view_get_title(view->GetWLCHandle())));
}

void ViewFocus(const FunctionCallbackInfo<Value>& info) {
  UNWRAP_VIEW
  wlc_view_focus(view->GetWLCHandle());
}

void ViewClose(const FunctionCallbackInfo<Value>& info) {
  UNWRAP_VIEW
  wlc_view_close(view->GetWLCHandle());
}

void ViewGetOutput(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  UNWRAP_VIEW
  wlc_handle output = wlc_view_get_output(view->GetWLCHandle());
  RETURN(info, Output::FromWLCHandle(output)->GetInstance());
}

void ViewSetOutput(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info) {
  UNWRAP_VIEW
  Output *output;
  Local<Object> output_js;
  if (!TryCast(value, &output_js)) THROW(TypeError, "view.output must be an object");
  if (!Unwrap(Output::FromLocalObject(output_js), &output)) return;
  wlc_view_set_output(view->GetWLCHandle(), output->GetWLCHandle());
}

void ViewSendToBack(const FunctionCallbackInfo<Value>& info) {
  UNWRAP_VIEW
  wlc_view_send_to_back(view->GetWLCHandle());
}

void ViewBringToFront(const FunctionCallbackInfo<Value>& info) {
  UNWRAP_VIEW
  wlc_view_bring_to_front(view->GetWLCHandle());
}

void ViewSetGeometry(const FunctionCallbackInfo<Value>& info) {
  ISOLATE(info);
  UNWRAP_VIEW
  wlc_geometry geometry;
  Local<Object> geometry_js;
  Local<Value> edge_js;

  if (!TryCast(info[0], &geometry_js)) THROW(TypeError, "view.setGeometry argument is not an Object");
  if (!TryCast(geometry_js, &geometry)) return;
  if (!Unwrap(geometry_js->Get(isolate->GetCurrentContext(), NewString("edge")), &edge_js)) return;

  uint32_t edge;
  if (edge_js->IsUndefined()) {
    edge = 0;
  }
  else if (!TryCast(edge_js, &edge)) {
    THROW(TypeError, "view.setGeometry edge option should be a Number");
  }

  wlc_view_set_geometry(view->GetWLCHandle(), edge, &geometry);
}

void View::InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl) {

  DefinePrototypeAccessor(isolate, tpl, "title", ViewGetTitle) &&
  DefinePrototypeMethod(isolate, tpl, "close", ViewClose) &&
  DefinePrototypeMethod(isolate, tpl, "focus", ViewFocus) &&
  DefinePrototypeAccessor(isolate, tpl, "output", ViewGetOutput, ViewSetOutput) &&
  DefinePrototypeMethod(isolate, tpl, "sendToBack", ViewSendToBack) &&
  DefinePrototypeMethod(isolate, tpl, "bringToFront", ViewBringToFront) &&
  DefinePrototypeMethod(isolate, tpl, "setGeometry", ViewSetGeometry);

}

}
