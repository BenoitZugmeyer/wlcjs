#ifndef _WLCJS_H
#define _WLCJS_H

extern "C" {
#include <wlc/wlc.h>
}

#include <node.h>
#include "uv.h"

namespace wlcjs {

using v8::Isolate;
using v8::HandleScope;
using v8::Object;
using v8::Local;
using v8::Value;
using v8::String;
using v8::Function;
using v8::Persistent;
using v8::Number;
using v8::Integer;
using v8::FunctionCallbackInfo;
using v8::Exception;
using v8::Array;

#define S(C) String::NewFromUtf8(isolate, C)

#define THROW(T, E) {\
    isolate->ThrowException(Exception::T(S(E)));\
    return;\
}

template <class T>
class SimplePersistent : public Persistent<T> {
public:

  template <class S>
  V8_INLINE void Reset(const Local<S>& other) {
    this->isolate = other->GetIsolate();
    Persistent<S>::Reset(this->isolate, other);
  }

  V8_INLINE Isolate* GetIsolate() {
    return this->isolate;
  }

private:
  Isolate* isolate;
};

wlc_interface* get_wlc_interface(Local<Object>);
bool is_initalized();

const char* enum_to_string(wlc_log_type);
const char* enum_to_string(wlc_backend_type);
const char* keysym_to_string(uint32_t);

}

#endif
