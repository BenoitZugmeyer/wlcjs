#include "output.h"

namespace wlcjs {

void OutputGetName(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  ISOLATE(info);
  GET_HANDLE(Output);
  RETURN(info, S(wlc_output_get_name(handle)));
}

void Output::Init(Local<Object> exports) {
  assert(constructor_.IsEmpty());
  ISOLATE(**exports)

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate);
  tpl->SetClassName(S("Output"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

  DEFINE_GETTER(prototype, "name", OutputGetName);

  constructor_.Reset(tpl->GetFunction());
  exports->Set(S("Output"), tpl->GetFunction());
}

}
