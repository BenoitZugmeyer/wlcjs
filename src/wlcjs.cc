#include <xkbcommon/xkbcommon.h>
#include "wlcjs.h"
#include "output.h"
#include "view.h"
#include "types.h"
#include "interface.h"
#include "enum_to_string.h"

namespace wlcjs {

enum state_t {
  STATE_UNINITIALIZED = 0x1,
  STATE_INITIALIZED = 0x2,
  STATE_RUNNING = 0x4,
};

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
    NewString(enum_to_string(type)),
    NewString(str),
  };
  log_handler->Call(isolate->GetCurrentContext(), Null(isolate), 2, arguments);
}

char* v8string_to_cstring(Local<String> str) {
  String::Utf8Value v(str);
  char* result = new char[v.length() + 1];
  memcpy(result, *v, v.length() + 1);
  return result;
}

METHOD(Init) {
  ISOLATE(info);
  if (~state & STATE_UNINITIALIZED) THROW(Error, "Can't call init twice");

  Local<Context> context = isolate->GetCurrentContext();
  Local<Object> interface;
  Local<Object> process;
  Local<Array> argv_js;

  if (!TryCast(info[0], &interface)) {
    THROW(TypeError, "wlc.init argument must be an Object");
  }

  if (!Unwrap(context->Global()->Get(context, NewString("process")), &process)) {
    THROW(Error, "global.process must be an Object");
  }

  if (!Unwrap(process->Get(context, NewString("argv")), &argv_js)) {
    THROW(TypeError, "global.process.argv must be an Array");
  }

  Local<String> str;
  int argc = argv_js->Length();
  char *argv[argc];
  for (int i = 0; i < argc; i += 1) {
    if (!Unwrap(argv_js->Get(context, i), &str)) {
      THROW(TypeError, "global.process.argv[%d] must be a String", i);
    }
    argv[i] = v8string_to_cstring(str);
  }

  wlc_log_set_handler(log_handler);

  if (!wlc_init(get_wlc_interface(isolate, interface), argc, argv)) {
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
  if (~state & STATE_INITIALIZED) THROW(Error, "'init' has to be called before calling 'run'");
  if (state & STATE_RUNNING) THROW(Error, "'run' can't be called twice");

  uv_timer_init(uv_default_loop(), &async_run_timer);
  uv_timer_start(&async_run_timer, run_cb, 0, 0);
}

METHOD(Terminate) {
  if (state & STATE_RUNNING) wlc_terminate();
}

METHOD(SetLogHandler) {
  ISOLATE(info);
  Local<Function> handler;
  if (!TryCast(info[0], &handler)) THROW(TypeError, "logHandler argument must be a Function");
  persistent_log_handler.Reset(isolate, handler);
}

METHOD(GetKeysymForKey) {
  ISOLATE(info)
  if (~state & STATE_INITIALIZED) THROW(Error, "'init' has to be called before calling 'getKeysymForKey'");

  // TODO modifiers support
  uint32_t key;
  if (!TryCast(info[0], &key)) THROW(TypeError, "getKeysymForKey argument must be a number");

  uint32_t keysym = wlc_keyboard_get_keysym_for_key(key, NULL);

  RETURN(info, Integer::NewFromUnsigned(isolate, keysym));
}

METHOD(GetKeysymNameForKey) {
  if (~state & STATE_INITIALIZED) THROW(Error, "'init' has to be called before calling 'getKeysymNameForKey'");

  // TODO modifiers support
  uint32_t key;
  if (!TryCast(info[0], &key)) THROW(TypeError, "getKeysymNameForKey argument must be a number");

  uint32_t keysym = wlc_keyboard_get_keysym_for_key(key, NULL);
  char buffer[100];
  if (xkb_keysym_get_name(keysym, buffer, 100) < 0) THROW(Error, "Invalid keysym");

  RETURN(info, NewString(buffer));
}

METHOD(GetBackendType) {
  if (~state & STATE_INITIALIZED) THROW(Error, "'init' has to be called before calling 'getBackendType'");
  RETURN(info, NewString(enum_to_string(wlc_get_backend_type())));
}

METHOD(GetOutputs) {
  ISOLATE(info)
  if (state & ~STATE_INITIALIZED) THROW(Error, "'init' has to be called before calling 'getOutputs'");
  size_t memb;
  const wlc_handle* outputs = wlc_get_outputs(&memb);
  Local<Array> result = Array::New(isolate, memb);
  Local<Context> context = isolate->GetCurrentContext();

  for (size_t i = 0; i < memb; i += 1) {
    if (result->Set(context, i, Output::FromWLCHandle(outputs[i])->GetInstance()).IsNothing()) {
      return;
    }
  }

  RETURN(info, result);
}

METHOD(Exec) {
  Local<String> bin_js;
  if (!TryCast(info[0], &bin_js)) {
    THROW(TypeError, "wlc.exec first argument must be a String");
  }

  char* bin = v8string_to_cstring(bin_js);

  size_t length = info.Length();
  char* argv[length + 1];

  Local<String> str;
  for (size_t i = 0; i < length; i += 1) {
    if (!TryCast(info[i], &str)) THROW(TypeError, "wlc.exec argument %d must be a String", i);
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
  Isolate* isolate = Isolate::GetCurrent();
  Output::Init(isolate, exports);
  View::Init(isolate, exports);
}

NODE_MODULE(addon, init)

}
