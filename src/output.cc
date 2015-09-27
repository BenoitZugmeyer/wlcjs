#include "output.h"

namespace wlcjs {

#define UNWRAP_OUTPUT \
  Output* output; \
  if (!Unwrap(Output::FromLocalObject(info.This()), &output)) return;

void OutputGetName(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  UNWRAP_OUTPUT
  RETURN(info, NewString(wlc_output_get_name(output->GetWLCHandle())));
}

void Output::InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl) {
  DEFINE_GETTER(tpl, "name", OutputGetName);
}

}
