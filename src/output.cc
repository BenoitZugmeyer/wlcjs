#include "output.h"

namespace wlcjs {

SimplePersistent<Function> Output::constructor;


void GetName(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  Isolate* isolate = info.GetIsolate();
  Output* output = static_cast<Output*>(info.This()->GetAlignedPointerFromInternalField(0));
  info.GetReturnValue().Set(S(wlc_output_get_name(output->output())));
}

void Output::Init(Local<Object> exports) {
  Isolate* isolate = exports->GetIsolate();

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate);
  tpl->SetClassName(S("Output"));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Local<ObjectTemplate> prototype = tpl->PrototypeTemplate();

  prototype->SetAccessor(
      S("name"),
      GetName,
      0,
      Local<Value>(),
      AccessControl::DEFAULT,
      PropertyAttribute::ReadOnly,
      AccessorSignature::New(isolate, tpl));

  constructor.Reset(tpl->GetFunction());
  exports->Set(S("Output"), tpl->GetFunction());
}

Output::Output(wlc_handle output) : output_(output) {
  assert(!constructor.IsEmpty());
  HandleScope scope(constructor.GetIsolate());
  Local<Object> result = constructor.Unwrap()->NewInstance();
  result->SetAlignedPointerInInternalField(0, this);
  instance_.Reset(result);
}

Local<Object> Output::instance() {
  assert(!instance_.IsEmpty());
  return instance_.Unwrap();
}

wlc_handle Output::output() {
  return output_;
}

Output::~Output() {
  instance_.Empty();
}

}
