#ifndef _WLCJS_UTIL_H
#define _WLCJS_UTIL_H

#include "common.h"
#include "types.h"

namespace wlcjs {

inline bool DefinePrototypeMethod(Isolate* isolate, Local<FunctionTemplate> tpl,
    const char* name, FunctionCallback callback) {

  Local<Function> fn;
  MaybeLocal<Function> maybe_fn =
    FunctionTemplate::New(
        isolate,
        callback,
        Local<Value>(),
        Signature::New(isolate, tpl)
    )->GetFunction(isolate->GetCurrentContext());

  if (!Unwrap(maybe_fn, &fn)) {
    ThrowException(Exception::Error, "Can't set function %s", name);
    return false;
  }

  Local<String> name_js = NewString(name);
  tpl->PrototypeTemplate()->Set(name_js, fn);
  fn->SetName(name_js);

  return true;
}

inline bool DefinePrototypeAccessor(Isolate* isolate, Local<FunctionTemplate> tpl,
    const char* name, AccessorGetterCallback getter, AccessorSetterCallback setter = 0) {

  tpl->PrototypeTemplate()->SetAccessor(
      NewString(name),
      getter,
      setter,
      Local<Value>(),
      AccessControl::DEFAULT,
      PropertyAttribute::None,
      AccessorSignature::New(isolate, tpl)
  );

  return true;
}

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

}

#endif
