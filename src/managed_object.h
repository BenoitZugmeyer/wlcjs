#ifndef _WLCJS_MANAGED_OBJECT_H
#define _WLCJS_MANAGED_OBJECT_H

#include "util.h"

namespace wlcjs {

// https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern

template<class T>
class ManagedObject {
 public:

  static void Init(Isolate* isolate, Local<Object> exports) {
    assert(constructor_.IsEmpty());

    Local<FunctionTemplate> tpl = FunctionTemplate::New(isolate);
    tpl->SetClassName(NewString(T::name));
    tpl->InstanceTemplate()->SetInternalFieldCount(1);

    T::InitPrototype(isolate, tpl);

    Local<Function> fn;
    if (!Unwrap(tpl->GetFunction(isolate->GetCurrentContext()), &fn)) return;

    constructor_.Reset(isolate, fn);
    if (exports->Set(isolate->GetCurrentContext(), NewString(T::name), fn).IsNothing()) {
      return;
    }
  }

  static Maybe<T*> Create(Isolate* isolate, wlc_handle handle) {
    assert(!constructor_.IsEmpty());
    Local<Object> instance;
    if (!Unwrap(constructor_.Unwrap()->NewInstance(isolate->GetCurrentContext()), &instance)) {
      return Nothing<T*>();
    }

    T* result = new T(handle);

    instance->SetAlignedPointerInInternalField(0, result);
    result->instance_.Reset(isolate, instance);

    return Just<T*>(result);
  }

  static Maybe<T*> FromLocalObject(Local<Object> value) {
    // TODO check instanceof
    if (value->InternalFieldCount() != 1) {
      ThrowException(Exception::Error, "Wrong underlying object");
      return Nothing<T*>();
    }

    T* managed_object = static_cast<T*>(value->GetAlignedPointerFromInternalField(0));
    if (!managed_object) {
      ThrowException(Exception::Error, "Underlying object doesn't exist anymore");
      return Nothing<T*>();
    }

    return Just(managed_object);
  }

  static T* FromWLCHandle(wlc_handle handle) {
    return static_cast<T*>(wlc_handle_get_user_data(handle));
  }


  ~ManagedObject() {
    HandleScope scope(constructor_.GetIsolate());
    instance_.Unwrap()->SetAlignedPointerInInternalField(0, NULL);
    instance_.Empty();
  }

  inline Local<Object> GetInstance() {
    assert(!instance_.IsEmpty());
    return instance_.Unwrap();
  }

  inline wlc_handle GetWLCHandle() {
    return handle_;
  }

 protected:
  explicit ManagedObject(wlc_handle handle) : handle_(handle) {
  };

  static SimplePersistent<Function> constructor_;

 private:
  wlc_handle handle_;
  SimplePersistent<Object> instance_;
};

template <typename T>
SimplePersistent<Function> ManagedObject<T>::constructor_;

}

#endif

