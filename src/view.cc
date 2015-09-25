#include "view.h"

namespace wlcjs {

void ViewGetTitle(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  ISOLATE(info);
  GET_HANDLE(View);
  RETURN(info, S(wlc_view_get_title(handle)));
}

void ViewFocus(const FunctionCallbackInfo<Value>& info) {
  ISOLATE(info);
  GET_HANDLE(View);
  wlc_view_focus(handle);
}

void ViewClose(const FunctionCallbackInfo<Value>& info) {
  ISOLATE(info);
  GET_HANDLE(View);
  wlc_view_close(handle);
}

void View::InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl) {
  DEFINE_GETTER(tpl, "title", ViewGetTitle);
  DEFINE_METHOD(tpl, "close", ViewClose);
  DEFINE_METHOD(tpl, "focus", ViewFocus);
}

}
