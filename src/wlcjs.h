#ifndef _WLCJS_H
#define _WLCJS_H

extern "C" {
#include <wlc/wlc.h>
}

#include <node.h>
#include <stdarg.h>
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
using v8::FunctionTemplate;
using v8::ObjectTemplate;
using v8::AccessorSignature;
using v8::PropertyCallbackInfo;
using v8::AccessControl;
using v8::PropertyAttribute;
using v8::Undefined;
using v8::Boolean;
using v8::Signature;
using v8::MaybeLocal;
using v8::Maybe;
using v8::Nothing;
using v8::Just;
using v8::NewStringType;
using v8::Context;
using v8::TryCatch;

#define ISOLATE(V) Isolate* isolate = (V).GetIsolate();

inline Local<String> NewString(const char* s) {
  return String::NewFromUtf8(Isolate::GetCurrent(), s, NewStringType::kInternalized).ToLocalChecked();
}

inline void ThrowException(Local<Value> (*ex)(Local<String>), const char* format, ...) {
  va_list args;
  va_start(args, format);

  char buffer[256];
  vsnprintf(buffer, 256, format, args);
  Isolate::GetCurrent()->ThrowException(ex(NewString(buffer)));
  va_end(args);
}

#define THROW(T, ...) do {\
  ThrowException(Exception::T, ##__VA_ARGS__);\
  return;\
} while(0)

#define RETURN(A, V) \
  (A).GetReturnValue().Set(V);\
  return;

#define METHOD(N) void N(const FunctionCallbackInfo<Value>& info)

#define DEFINE_METHOD(TPL, NAME, FN) do {\
  Local<Function> fn;\
  MaybeLocal<Function> maybe_fn =\
    FunctionTemplate::New(\
      isolate,\
      FN,\
      Local<Value>(),\
      Signature::New(isolate, TPL)\
    )->GetFunction(isolate->GetCurrentContext());\
  if (!Unwrap(maybe_fn, &fn)) THROW(Error, "Can't set function " NAME);\
  TPL->PrototypeTemplate()->Set(NewString(NAME), fn);\
  fn->SetName(NewString(NAME));\
} while(0);

#define DEFINE_ACCESSOR(TPL, NAME, GETTER, SETTER) \
  TPL->PrototypeTemplate()->SetAccessor(\
      NewString(NAME),\
      GETTER,\
      SETTER,\
      Local<Value>(),\
      AccessControl::DEFAULT,\
      PropertyAttribute::None,\
      AccessorSignature::New(isolate, TPL));

#define DEFINE_GETTER(PROTO, NAME, GETTER) DEFINE_ACCESSOR(PROTO, NAME, GETTER, 0)

template <class T>
class SimplePersistent : public Persistent<T> {
public:

  template <class S>
  V8_INLINE void Reset(Isolate* isolate, const Local<S>& other) {
    this->isolate = isolate;
    Persistent<S>::Reset(isolate, other);
  }

  V8_INLINE Isolate* GetIsolate() {
    return this->isolate;
  }

  V8_INLINE Local<T> Unwrap() {
    return Local<T>::New(this->isolate, *this);
  }

private:
  Isolate* isolate;
};

wlc_interface* get_wlc_interface(Isolate*, Local<Object>);

const char* enum_to_string(wlc_log_type);
const char* enum_to_string(wlc_backend_type);
const char* enum_to_string(wlc_key_state);
const char* enum_to_string(wlc_button_state);

}

#endif
