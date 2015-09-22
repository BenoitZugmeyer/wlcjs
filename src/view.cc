#include "view.h"

namespace wlcjs {

void ViewGetTitle(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  ISOLATE(info);
  GET_HANDLE(View);
  RETURN(info, S(wlc_view_get_title(handle)));
}

void View::Init(Local<Object> exports) {
  assert(constructor_.IsEmpty());
  ISOLATE(**exports)

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate);
  tpl->SetClassName(S("View"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

  DEFINE_GETTER(prototype, "title", ViewGetTitle);

  constructor_.Reset(tpl->GetFunction());
  exports->Set(S("View"), tpl->GetFunction());
}

}
