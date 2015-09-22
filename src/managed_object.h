#ifndef _WLCJS_MANAGED_OBJECT_H
#define _WLCJS_MANAGED_OBJECT_H

#include "wlcjs.h"

namespace wlcjs {

// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern

template<class T>
class ManagedObject {
 public:
  explicit ManagedObject(wlc_handle);

  ~ManagedObject() {
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
ManagedObject<T>::ManagedObject(wlc_handle handle) : handle_(handle) {
  assert(!constructor_.IsEmpty());
  HandleScope scope(constructor_.GetIsolate());
  Local<Object> result = constructor_.Unwrap()->NewInstance();
  result->SetAlignedPointerInInternalField(0, this);
  instance_.Reset(result);
}

#define DEFINE_ACCESSOR(PROTO, NAME, GETTER, SETTER) \
  PROTO->SetAccessor(\
      S(NAME),\
      GETTER,\
      SETTER,\
      Local<Value>(),\
      AccessControl::DEFAULT,\
      PropertyAttribute::ReadOnly,\
      AccessorSignature::New(isolate, tpl));

#define DEFINE_GETTER(PROTO, NAME, GETTER) DEFINE_ACCESSOR(PROTO, NAME, GETTER, 0)

#define GET_HANDLE(TYPE)\
  wlc_handle handle = static_cast<TYPE*>(info.This()->GetAlignedPointerFromInternalField(0))->GetWLCHandle();

}

#endif

