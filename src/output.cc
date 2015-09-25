#include "output.h"

namespace wlcjs {

void OutputGetName(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  ISOLATE(info);
  GET_HANDLE(Output);
  RETURN(info, S(wlc_output_get_name(handle)));
}

void Output::InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl) {
  DEFINE_GETTER(tpl, "name", OutputGetName);
}

}
