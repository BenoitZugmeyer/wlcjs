#include "wlcjs.h"
#include "output.h"

namespace wlcjs {

wlc_event_source* timer = NULL;
uv_timer_t async_run_timer;
SimplePersistent<Function> persistent_log_handler;

int run_uv_loop(void *args) {
  uv_run(uv_default_loop(), UV_RUN_NOWAIT);
  wlc_event_source_timer_update(timer, 1);
  return 1;
}

void log_handler(enum wlc_log_type type, const char *str) {
  if (persistent_log_handler.IsEmpty()) return;
  ISOLATE(persistent_log_handler)
  HandleScope scope(isolate);
  Local<Function> log_handler = persistent_log_handler.Unwrap();
  Local<Value> arguments[] = {
    S(enum_to_string(type)),
    S(str),
  };
  log_handler->Call(Null(isolate), 2, arguments);
}

METHOD(Init) {
  ISOLATE(args)
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
}

void run_cb(uv_timer_t* handle) {
  timer = wlc_event_loop_add_timer(run_uv_loop, NULL);
  wlc_event_source_timer_update(timer, 1);
  wlc_run();
}

METHOD(Run) {
  ISOLATE(args)
  if (!is_initalized()) THROW(Error, "Call init first");

  uv_timer_init(uv_default_loop(), &async_run_timer);
  uv_timer_start(&async_run_timer, run_cb, 0, 0);
}

METHOD(SetLogHandler) {
  ISOLATE(args)
  ARG(0, Function, handler);

  persistent_log_handler.Reset(handler);
}

METHOD(GetKeysymForKey) {
  ISOLATE(args)
  ARG(0, Number, key);

  uint32_t keysym = wlc_keyboard_get_keysym_for_key(key->Uint32Value(), NULL);

  RETURN(args, Integer::NewFromUnsigned(isolate, keysym));
}

METHOD(GetKeysymStringForKey) {
  ISOLATE(args)
  ARG(0, Number, key);

  uint32_t keysym = wlc_keyboard_get_keysym_for_key(key->Uint32Value(), NULL);

  RETURN(args, S(keysym_to_string(keysym)));
}

METHOD(GetBackendType) {
  ISOLATE(args)
  RETURN(args, S(enum_to_string(wlc_get_backend_type())));
}

METHOD(GetOutputs) {
  ISOLATE(args)
  size_t memb;
  const wlc_handle* outputs = wlc_get_outputs(&memb);
  Local<Array> result = Array::New(isolate, memb);

  for (size_t i = 0; i < memb; i += 1) {
    Output* output = static_cast<Output*>(wlc_handle_get_user_data(outputs[i]));
    result->Set(i, output->instance());
  }

  RETURN(args, result);
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "init", Init);
  NODE_SET_METHOD(exports, "run", Run);
  NODE_SET_METHOD(exports, "setLogHandler", SetLogHandler);
  NODE_SET_METHOD(exports, "getKeysymForKey", GetKeysymForKey);
  NODE_SET_METHOD(exports, "getKeysymStringForKey", GetKeysymStringForKey);
  NODE_SET_METHOD(exports, "getBackendType", GetBackendType);
  NODE_SET_METHOD(exports, "getOutputs", GetOutputs);
  Output::Init(exports);
}

NODE_MODULE(addon, init)

}
