#include "view.h"
#include "output.h"

namespace wlcjs {

void ViewGetTitle(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  UNWRAP_OR(view, View::FromLocalObject(info.This()), return);
  RETURN(info, NewString(wlc_view_get_title(view->GetWLCHandle())));
}

void ViewFocus(const FunctionCallbackInfo<Value>& info) {
  UNWRAP_OR(view, View::FromLocalObject(info.This()), return);
  wlc_view_focus(view->GetWLCHandle());
}

void ViewClose(const FunctionCallbackInfo<Value>& info) {
  UNWRAP_OR(view, View::FromLocalObject(info.This()), return);
  wlc_view_close(view->GetWLCHandle());
}

void ViewGetOutput(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  UNWRAP_OR(view, View::FromLocalObject(info.This()), return);
  wlc_handle output = wlc_view_get_output(view->GetWLCHandle());
  RETURN(info, Output::FromWLCHandle(output)->GetInstance());
}

void ViewSetOutput(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info) {
  UNWRAP_OR(view, View::FromLocalObject(info.This()), return);
  GET_LOCAL_AS(Object, output_js, value, "view.output");
  UNWRAP_OR(output, Output::FromLocalObject(output_js), return);
  wlc_view_set_output(view->GetWLCHandle(), output->GetWLCHandle());
}

void View::InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl) {
  DEFINE_GETTER(tpl, "title", ViewGetTitle);
  DEFINE_METHOD(tpl, "close", ViewClose);
  DEFINE_METHOD(tpl, "focus", ViewFocus);
  DEFINE_ACCESSOR(tpl, "output", ViewGetOutput, ViewSetOutput);
}

}
