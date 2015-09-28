#ifndef _WLCJS_UTIL_H
#define _WLCJS_UTIL_H

#include "common.h"

namespace wlcjs {

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

}

#endif
