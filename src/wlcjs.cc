// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#include "./common.h"
#include "./callbacks.h"
#include "./input.h"
#include "./output.h"
#include "./view.h"
#include "./core.h"

namespace wlcjs {

void init_ns(
    Local<Object> exports,
    const char* name, void (*Export)(Local<Object>)) {
  Isolate* isolate = Isolate::GetCurrent();
  Local<Context> context = isolate->GetCurrentContext();

  Local<Object> ns = Object::New(isolate);
  Export(ns);
  exports->Set(context, NewString(name), ns);
}

void init(Local<Object> exports) {
  Core::Export(exports);
  init_ns(exports, "callbacks", Callbacks::Export);
  init_ns(exports, "input", Input::Export);
  init_ns(exports, "view", View::Export);
  init_ns(exports, "output", Output::Export);
}

NODE_MODULE(addon, init)

}  // namespace wlcjs
