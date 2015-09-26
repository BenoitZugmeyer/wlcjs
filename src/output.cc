#include "output.h"

namespace wlcjs {

void OutputGetName(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  UNWRAP_OR(output, Output::FromLocalObject(info.This()), return);
  RETURN(info, NewString(wlc_output_get_name(output->GetWLCHandle())));
}

void Output::InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl) {
  DEFINE_GETTER(tpl, "name", OutputGetName);
}

}
