// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#include "./types.h"
#include "./util.h"
#include "./state.h"

namespace wlcjs {
namespace Core {

wlc_event_source* timer = NULL;
uv_timer_t async_run_timer;
SimplePersistent<Function> persistent_log_handler;

void log_handler(enum wlc_log_type type, const char *str) {
  if (persistent_log_handler.IsEmpty()) return;
  ISOLATE(persistent_log_handler)
  HandleScope scope(isolate);
  Local<Function> log_handler = persistent_log_handler.Unwrap();
  Local<Value> arguments[] = {
    Number::New(isolate, type),
    NewString(str),
  };
  log_handler->Call(isolate->GetCurrentContext(), Null(isolate), 2, arguments);
}

METHOD(SetLogHandler) {
  ISOLATE(info);
  Local<Function> handler;
  if (!TryCast(info[0], &handler)) {
    THROW(TypeError, "logHandler argument must be a Function");
  }
  persistent_log_handler.Reset(isolate, handler);
}

METHOD(Init) {
  if (~state & STATE_UNINITIALIZED) THROW(Error, "Can't call init twice");

  wlc_log_set_handler(log_handler);

  if (!wlc_init()) THROW(Error, "Failed to initialize wlc");

  state = STATE_INITIALIZED;
}

METHOD(Terminate) {
  if (state & STATE_RUNNING) wlc_terminate();
  state = STATE_TERMINATED;
}

METHOD(GetBackendType) {
  ISOLATE(info)
  if (~state & STATE_INITIALIZED) {
    THROW(Error, "'init' has to be called before calling 'getBackendType'");
  }
  RETURN(Number::New(isolate, wlc_get_backend_type()));
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
    if (!TryCast(info[i], &str)) {
      THROW(TypeError, "wlc.exec argument %d must be a String", i);
    }
    argv[i] = v8string_to_cstring(str);
  }
  argv[length] = NULL;

  wlc_exec(bin, argv);

  for (size_t i = 0; i < length; i += 1) {
    delete argv[i];
  }
  delete bin;
}

int run_uv_loop(void *args) {
  uv_run(uv_default_loop(), UV_RUN_NOWAIT);
  wlc_event_source_timer_update(timer, 1);
  return 1;
}

void run_cb(uv_timer_t* handle) {
  timer = wlc_event_loop_add_timer(run_uv_loop, NULL);
  wlc_event_source_timer_update(timer, 1);
  wlc_run();
}

METHOD(Run) {
  if (~state & STATE_INITIALIZED) {
    THROW(Error, "'init' has to be called before calling 'run'");
  }
  if (state & STATE_RUNNING) {
    THROW(Error, "'run' can't be called twice");
  }

  state = STATE_RUNNING | STATE_INITIALIZED;

  uv_timer_init(uv_default_loop(), &async_run_timer);
  uv_timer_start(&async_run_timer, run_cb, 0, 0);
}

METHOD(GetOutputs) {
  if (~state & STATE_INITIALIZED) {
    THROW(Error, "'init' has to be called before calling 'getOutputs'");
  }

  size_t memb;
  Local<Array> result;

  const wlc_handle* outputs = wlc_get_outputs(&memb);
  if (!TryCast(outputs, memb, &result)) return;

  RETURN(result);
}

METHOD(GetFocusedOutput) {
  ISOLATE(info)
  RETURN(Number::New(isolate, wlc_get_focused_output()));
}

void Export(Local<Object> exports) {
  NODE_SET_METHOD(exports, "setLogHandler", SetLogHandler);
  NODE_SET_METHOD(exports, "init", Init);
  NODE_SET_METHOD(exports, "terminate", Terminate);
  NODE_SET_METHOD(exports, "getBackendType", GetBackendType);
  NODE_SET_METHOD(exports, "exec", Exec);
  NODE_SET_METHOD(exports, "run", Run);

  NODE_SET_METHOD(exports, "getOutputs", GetOutputs);
  NODE_SET_METHOD(exports, "getFocusedOutput", GetFocusedOutput);
}

}  // namespace Core
}  // namespace wlcjs
