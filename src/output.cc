#include "output.h"

namespace wlcjs {

#define UNWRAP_OUTPUT \
  Output* output; \
  if (!Unwrap(Output::FromLocalObject(info.This()), &output)) return;

void OutputGetName(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  UNWRAP_OUTPUT
  RETURN(info, NewString(wlc_output_get_name(output->GetWLCHandle())));
}

void OutputGetViews(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  ISOLATE(info);
  UNWRAP_OUTPUT

  size_t memb;
  const wlc_handle* views = wlc_output_get_views(output->GetWLCHandle(), &memb);
  Local<Array> result = Array::New(isolate, memb);
  Local<Context> context = isolate->GetCurrentContext();

  for (size_t i = 0; i < memb; i += 1) {
    if (result->Set(context, i, Output::FromWLCHandle(views[i])->GetInstance()).IsNothing()) {
      return;
    }
  }

  RETURN(info, result);
}

void OutputGetResolution(Local<String> property, const PropertyCallbackInfo<Value>& info) {
  ISOLATE(info);
  UNWRAP_OUTPUT

  const wlc_size* resolution = wlc_output_get_resolution(output->GetWLCHandle());
  if (!resolution) return;

  Local<Object> resolution_js;
  if (!Unwrap(Convert(isolate, resolution), &resolution_js)) return;

  RETURN(info, resolution_js);
}

void Output::InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl) {
  DEFINE_GETTER(tpl, "name", OutputGetName);
  DEFINE_GETTER(tpl, "views", OutputGetViews);
  DEFINE_GETTER(tpl, "resolution", OutputGetResolution);
}

}
