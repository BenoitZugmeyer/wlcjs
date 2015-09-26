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

#define MK_VARIADIC(ORIGIN, LAST_ARG) {\
  va_list args; \
  va_start(args, LAST_ARG); \
  auto result = ORIGIN(LAST_ARG, args); \
  va_end(args); \
  return result;\
}

#define MK_VARIADIC_N(ORIGIN, LAST_ARG, ...) {\
  va_list args; \
  va_start(args, LAST_ARG); \
  auto result = ORIGIN(__VA_ARGS__, LAST_ARG, args); \
  va_end(args); \
  return result;\
}


inline Local<String> NewString(const char* s) {
  return String::NewFromUtf8(Isolate::GetCurrent(), s, NewStringType::kInternalized).ToLocalChecked();
}

inline Local<String> NewStringFormated(const char* format, va_list args) {
  char buffer[256];
  vsnprintf(buffer, 256, format, args);
  return NewString(buffer);
}

inline Local<String> NewStringFormated(const char* format, ...)
  MK_VARIADIC(NewStringFormated, format)

inline void ThrowException(Local<Value> (*ex)(Local<String>), const char* format, ...) {
  va_list args;
  va_start(args, format);
  Isolate::GetCurrent()->ThrowException(ex(NewStringFormated(format, args)));
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
  GET_LOCAL_AS(Function, fn, FunctionTemplate::New(\
      isolate,\
      FN,\
      Local<Value>(),\
      Signature::New(isolate, TPL))->GetFunction(isolate->GetCurrentContext()))\
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

}

#endif
