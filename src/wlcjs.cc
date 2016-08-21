#include "common.h"

namespace wlcjs {

void Export(Local<Object> exports);

namespace View {
void Export(Local<Object> exports);
}

namespace Output {
void Export(Local<Object> exports);
}

namespace Callbacks {
void Export(Local<Object> exports);
}

void init_ns(Local<Object> exports, const char* name, void (*Export)(Local<Object>)) {
  Isolate* isolate = Isolate::GetCurrent();
  Local<Context> context = isolate->GetCurrentContext();

  Local<Object> ns = Object::New(isolate);
  Export(ns);
  exports->Set(context, NewString(name), ns);
}

void init(Local<Object> exports) {
  Export(exports);
  init_ns(exports, "callbacks", Callbacks::Export);
  init_ns(exports, "view", View::Export);
  init_ns(exports, "output", Output::Export);
}

NODE_MODULE(addon, init)

}
