#include <xkbcommon/xkbcommon.h>
#include "wlcjs.h"
#include "output.h"

namespace wlcjs {

enum state_t {
  STATE_UNINITIALIZED = 0x1,
  STATE_INITIALIZED = 0x2,
  STATE_RUNNING = 0x4,
};

#define ASSERT_STATE(S) if (!(state & STATE_ ## S)) THROW(Error, "State should be " #S);
#define ASSERT_NOT_STATE(S) if (state & STATE_ ## S) THROW(Error, "State should not be " #S);

wlc_event_source* timer = NULL;
uv_timer_t async_run_timer;
SimplePersistent<Function> persistent_log_handler;
int state = STATE_UNINITIALIZED;

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

char* v8string_to_cstring(Local<String> str) {
  String::Utf8Value v(str);
  char* result = new char[v.length() + 1];
  memcpy(result, *v, v.length() + 1);
  return result;
}

METHOD(Init) {
  ISOLATE(args)
  ASSERT_STATE(UNINITIALIZED);
  ARG(0, Object, interface);

  Local<Object> global = isolate->GetCurrentContext()->Global();
  GET_AS(Object, global->Get(S("process")), process, "process");
  GET_AS(Array, process->Get(S("argv")), jsArgv, "process.argv");

  int argc = jsArgv->Length();
  char *argv[argc];
  for (int i = 0; i < argc; i += 1) {
    GET_AS(String, jsArgv->Get(i), str, "process.argv[%d]", i);
    argv[i] = v8string_to_cstring(str);
  }

  wlc_log_set_handler(log_handler);

  if (!wlc_init(get_wlc_interface(interface), argc, argv)) {
    THROW(Error, "Failed to initialize wlc");
  }
  state = STATE_INITIALIZED;
}

void run_cb(uv_timer_t* handle) {
  if (state & STATE_RUNNING) return;

  state = STATE_RUNNING | STATE_INITIALIZED;

  timer = wlc_event_loop_add_timer(run_uv_loop, NULL);
  wlc_event_source_timer_update(timer, 1);
  wlc_run();
}

METHOD(Run) {
  ISOLATE(args)
  ASSERT_STATE(INITIALIZED);
  ASSERT_NOT_STATE(RUNNING);

  uv_timer_init(uv_default_loop(), &async_run_timer);
  uv_timer_start(&async_run_timer, run_cb, 0, 0);
}

METHOD(Terminate) {
  if (state & STATE_RUNNING) wlc_terminate();
}

METHOD(SetLogHandler) {
  ISOLATE(args)
  ARG(0, Function, handler);

  persistent_log_handler.Reset(handler);
}

METHOD(GetKeysymForKey) {
  ISOLATE(args)
  ASSERT_STATE(INITIALIZED);
  ARG(0, Number, key);

  // TODO modifiers support
  uint32_t keysym = wlc_keyboard_get_keysym_for_key(key->Uint32Value(), NULL);

  RETURN(args, Integer::NewFromUnsigned(isolate, keysym));
}

METHOD(GetKeysymNameForKey) {
  ISOLATE(args)
  ASSERT_STATE(INITIALIZED);
  ARG(0, Number, key);

  // TODO modifiers support
  uint32_t keysym = wlc_keyboard_get_keysym_for_key(key->Uint32Value(), NULL);
  char buffer[100];
  if (xkb_keysym_get_name(keysym, buffer, 100) < 0) THROW(Error, "Invalid keysym");

  RETURN(args, S(buffer));
}

METHOD(GetBackendType) {
  ISOLATE(args)
  ASSERT_STATE(INITIALIZED);
  RETURN(args, S(enum_to_string(wlc_get_backend_type())));
}

METHOD(GetOutputs) {
  ISOLATE(args)
  ASSERT_STATE(INITIALIZED);
  size_t memb;
  const wlc_handle* outputs = wlc_get_outputs(&memb);
  Local<Array> result = Array::New(isolate, memb);

  for (size_t i = 0; i < memb; i += 1) {
    Output* output = static_cast<Output*>(wlc_handle_get_user_data(outputs[i]));
    result->Set(i, output->GetInstance());
  }

  RETURN(args, result);
}

METHOD(Exec) {
  ISOLATE(args);
  ARG(0, String, jsbin);

  char* bin = v8string_to_cstring(jsbin);

  size_t length = args.Length();
  char* argv[length + 1];

  for (size_t i = 0; i < length; i += 1) {
    ARG(i, String, str);
    argv[i] = v8string_to_cstring(str);
  }
  argv[length] = NULL;

  wlc_exec(bin, argv);

  for (size_t i = 0; i < length; i += 1) {
    delete argv[i];
  }
  delete bin;
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "init", Init);
  NODE_SET_METHOD(exports, "run", Run);
  NODE_SET_METHOD(exports, "terminate", Terminate);
  NODE_SET_METHOD(exports, "setLogHandler", SetLogHandler);
  NODE_SET_METHOD(exports, "getKeysymForKey", GetKeysymForKey);
  NODE_SET_METHOD(exports, "getKeysymNameForKey", GetKeysymNameForKey);
  NODE_SET_METHOD(exports, "getBackendType", GetBackendType);
  NODE_SET_METHOD(exports, "getOutputs", GetOutputs);
  NODE_SET_METHOD(exports, "exec", Exec);
  Output::Init(exports);
}

NODE_MODULE(addon, init)

}
