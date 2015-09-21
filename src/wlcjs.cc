#include "wlcjs.h"

namespace wlcjs {

wlc_event_source* timer = NULL;
uv_timer_t async_run_timer;
SimplePersistent<Function> persistent_log_handler;

#define ARG(I, TYPE, NAME) \
  if (args.Length() <= I) THROW(TypeError, "Argument " #I " is required");\
  if (!args[I]->Is ## TYPE ()) THROW(TypeError, "Argument " #I " must be a " #TYPE);\
  Local<TYPE> NAME = args[I].As<TYPE>();

#define RETURN(V) \
  args.GetReturnValue().Set(V);\
  return;

#define FN(NAME, CODE) \
  void NAME (const FunctionCallbackInfo<Value>& args) {\
    Isolate* isolate = args.GetIsolate();\
    CODE\
  }

int run_uv_loop(void *args) {
  uv_run(uv_default_loop(), UV_RUN_NOWAIT);
  wlc_event_source_timer_update(timer, 1);
  return 1;
}

void log_handler(enum wlc_log_type type, const char *str) {
  if (persistent_log_handler.IsEmpty()) return;
  Isolate* isolate = persistent_log_handler.GetIsolate();
  HandleScope scope(isolate);
  Local<Function> log_handler = persistent_log_handler.Unwrap();
  Local<Value> arguments[] = {
    S(enum_to_string(type)),
    S(str),
  };
  log_handler->Call(Null(isolate), 2, arguments);
}

FN(Init,
  ARG(0, Object, interface);

  if (is_initalized()) THROW(Error, "Can't call init twice");

  Local<Array> jsArgv = isolate->GetCurrentContext()->Global()
    ->Get(S("process")).As<Object>()
    ->Get(S("argv")).As<Array>();

  int argc = jsArgv->Length();
  char *argv[argc];
  for (int i = 0; i < argc; i += 1) {
    String::Utf8Value v(jsArgv->Get(i)->ToString());
    argv[i] = new char[v.length() + 1];
    memcpy(argv[i], *v, v.length() + 1);
  }

  wlc_log_set_handler(log_handler);

  wlc_init(get_wlc_interface(interface), argc, argv);
)

void run_cb(uv_timer_t* handle) {
  timer = wlc_event_loop_add_timer(run_uv_loop, NULL);
  wlc_event_source_timer_update(timer, 1);
  wlc_run();
}

FN(Run,
  if (!is_initalized()) THROW(Error, "Call init first");

  uv_timer_init(uv_default_loop(), &async_run_timer);
  uv_timer_start(&async_run_timer, run_cb, 0, 0);
)

FN(SetLogHandler,
  ARG(0, Function, handler);

  persistent_log_handler.Reset(handler);
)

FN(GetKeysymForKey,
  ARG(0, Number, key);

  uint32_t keysym = wlc_keyboard_get_keysym_for_key(key->Uint32Value(), NULL);

  RETURN(Integer::NewFromUnsigned(isolate, keysym));
)

FN(GetKeysymStringForKey,
  ARG(0, Number, key);

  uint32_t keysym = wlc_keyboard_get_keysym_for_key(key->Uint32Value(), NULL);

  RETURN(S(keysym_to_string(keysym)));
)

FN(GetBackendType,
  RETURN(S(enum_to_string(wlc_get_backend_type())));
)

FN(GetOutputs,
  size_t memb;
  const wlc_handle* outputs = wlc_get_outputs(&memb);
  Local<Array> result = Array::New(isolate, memb);

  for (size_t i = 0; i < memb; i += 1) {
    result->Set(i, Integer::NewFromUnsigned(isolate, outputs[i]));
  }

  RETURN(result);
)

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "init", Init);
  NODE_SET_METHOD(exports, "run", Run);
  NODE_SET_METHOD(exports, "setLogHandler", SetLogHandler);
  NODE_SET_METHOD(exports, "getKeysymForKey", GetKeysymForKey);
  NODE_SET_METHOD(exports, "getKeysymStringForKey", GetKeysymStringForKey);
  NODE_SET_METHOD(exports, "getBackendType", GetBackendType);
  NODE_SET_METHOD(exports, "getOutputs", GetOutputs);
}

NODE_MODULE(addon, init)

}
