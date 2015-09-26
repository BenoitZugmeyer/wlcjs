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

void View::InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl) {
  DEFINE_GETTER(tpl, "title", ViewGetTitle);
  DEFINE_METHOD(tpl, "close", ViewClose);
  DEFINE_METHOD(tpl, "focus", ViewFocus);
}

}
