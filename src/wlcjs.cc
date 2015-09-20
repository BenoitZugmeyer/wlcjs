#include "wlcjs.h"

namespace wlcjs {

wlc_event_source* timer = NULL;
uv_timer_t async_run_timer;

int run_uv_loop(void *args) {
  uv_run(uv_default_loop(), UV_RUN_NOWAIT);
  wlc_event_source_timer_update(timer, 1);
  return 1;
}

void Init(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (is_initalized()) THROW(Error, "Can't call init twice");
  if (args.Length() < 1) THROW(TypeError, "'interface' argument required");
  if (!args[0]->IsObject()) THROW(TypeError, "'interface' must be an object");

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

  wlc_init(get_wlc_interface(args[0]->ToObject()), argc, argv);
}

void run_cb(uv_timer_t* handle) {
  timer = wlc_event_loop_add_timer(run_uv_loop, NULL);
  wlc_event_source_timer_update(timer, 1);
  wlc_run();
}

void Run(const FunctionCallbackInfo<Value>& args) {
  Isolate* isolate = args.GetIsolate();

  if (!is_initalized()) THROW(Error, "Call init first");

  uv_timer_init(uv_default_loop(), &async_run_timer);
  uv_timer_start(&async_run_timer, run_cb, 0, 0);
}

void init(Local<Object> exports) {
  NODE_SET_METHOD(exports, "init", Init);
  NODE_SET_METHOD(exports, "run", Run);
}

NODE_MODULE(addon, init)

}
