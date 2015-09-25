#ifndef _WLCJS_MANAGED_OBJECT_H
#define _WLCJS_MANAGED_OBJECT_H

#include "wlcjs.h"

namespace wlcjs {

// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern

template<class T>
class ManagedObject {
 public:
  static void Init(Local<Object> exports);

  explicit ManagedObject(wlc_handle);

  ~ManagedObject() {
    HandleScope scope(constructor_.GetIsolate());
    instance_.Unwrap()->SetAlignedPointerInInternalField(0, NULL);
    instance_.Empty();
  }

  inline Local<Object> GetInstance() {
    assert(!instance_.IsEmpty());
    return instance_.Unwrap();
  }

  inline wlc_handle GetWLCHandle() { return handle_; }

 protected:
  static SimplePersistent<Function> constructor_;

 private:
  wlc_handle handle_;
  SimplePersistent<Object> instance_;
};

template <typename T>
SimplePersistent<Function> ManagedObject<T>::constructor_;

template <typename T>
void ManagedObject<T>::Init(Local<Object> exports) {
  assert(constructor_.IsEmpty());
  ISOLATE(**exports)

  Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate);
  tpl->SetClassName(S(T::name));
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  T::InitPrototype(isolate, tpl);

  constructor_.Reset(tpl->GetFunction());
  exports->Set(S(T::name), tpl->GetFunction());
}

template <typename T>
ManagedObject<T>::ManagedObject(wlc_handle handle) : handle_(handle) {
  assert(!constructor_.IsEmpty());
  HandleScope scope(constructor_.GetIsolate());
  Local<Object> result = constructor_.Unwrap()->NewInstance();
  result->SetAlignedPointerInInternalField(0, this);
  instance_.Reset(result);
}

#define DEFINE_METHOD(TPL, NAME, FN) do {\
  Local<Function> fn = FunctionTemplate::New(\
      isolate,\
      FN,\
      Local<Value>(),\
      Signature::New(isolate, TPL))->GetFunction();\
  TPL->PrototypeTemplate()->Set(S(NAME), fn);\
  fn->SetName(S(NAME));\
} while(0);

#define DEFINE_ACCESSOR(TPL, NAME, GETTER, SETTER) \
  TPL->PrototypeTemplate()->SetAccessor(\
      S(NAME),\
      GETTER,\
      SETTER,\
      Local<Value>(),\
      AccessControl::DEFAULT,\
      PropertyAttribute::ReadOnly,\
      AccessorSignature::New(isolate, TPL));

#define DEFINE_GETTER(PROTO, NAME, GETTER) DEFINE_ACCESSOR(PROTO, NAME, GETTER, 0)

#define GET_HANDLE(TYPE)\
  TYPE* managed_object = static_cast<TYPE*>(info.This()->GetAlignedPointerFromInternalField(0));\
  if (!managed_object) THROW(Error, "Underlying " #TYPE " doesn't exist anymore");\
  wlc_handle handle = managed_object->GetWLCHandle();

}

#endif

