#include "view.h"
#include "output.h"

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

void View::InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl) {
  DEFINE_GETTER(tpl, "title", ViewGetTitle);
  DEFINE_METHOD(tpl, "close", ViewClose);
  DEFINE_METHOD(tpl, "focus", ViewFocus);
  DEFINE_ACCESSOR(tpl, "output", ViewGetOutput, ViewSetOutput);
  DEFINE_METHOD(tpl, "sendToBack", ViewSendToBack);
}

}
