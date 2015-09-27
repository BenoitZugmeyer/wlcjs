#ifndef _TYPES_H
#define _TYPES_H

#include "wlcjs.h"

namespace wlcjs {

#define _DEFINE_TYPE_NAME(TYPE)                     \
  template <>                                       \
  inline const char* GetTypeName<TYPE>() {          \
    return #TYPE;                                   \
  }

#define _DEFINE_IS_TYPE(TYPE)                       \
  template <>                                       \
  inline bool IsType<TYPE>(Local<Value>& other) {   \
    return other->Is ## TYPE ();                    \
  }

#define _DEFINE(TYPE)                               \
  _DEFINE_TYPE_NAME(TYPE)                           \
  _DEFINE_IS_TYPE(TYPE)

template< typename T >
struct always_false {
  enum { value = false };
};

template <class T>
inline const char* GetTypeName() {
  static_assert(always_false<T>::value, "Unknown type");
  return NULL;
}

template <class T>
inline bool IsType(Local<Value>&) {
  static_assert(always_false<T>::value, "Unknown type");
  return false;
}

template <>
inline bool IsType<Value>(Local<Value>&) {
  return true;
}

_DEFINE(Object)
_DEFINE(Array)
_DEFINE(String)
_DEFINE(Number)
_DEFINE(Function)

template <class T>
inline bool Unwrap(Maybe<T> value, T* dest) {
  if (value.IsNothing()) return false;
  *dest = value.FromJust();
  return true;
};

template <class T>
inline bool TryCast(Local<Value> value, Local<T>* dest) {
  if (!IsType<T>(value)) return false;
  *dest = value.As<T>();
  return true;
}

inline bool TryCast(Local<Value> value, uint32_t* dest) {
  if (!IsType<Number>(value)) return false;
  return Unwrap(value->Uint32Value(Isolate::GetCurrent()->GetCurrentContext()), dest);
}

inline bool TryCast(Local<Value> value, int32_t* dest) {
  if (!IsType<Number>(value)) return false;
  return Unwrap(value->Int32Value(Isolate::GetCurrent()->GetCurrentContext()), dest);
}

template <class INPUT, class OUTPUT>
inline bool Unwrap(MaybeLocal<INPUT> maybe, OUTPUT* dest) {
  if (maybe.IsEmpty()) return false;
  return TryCast(maybe.ToLocalChecked(), dest);
}

inline bool TryCast(Local<Object> desc, wlc_geometry* geometry) {
  auto context = Isolate::GetCurrent()->GetCurrentContext();

  return (
    Unwrap(desc->Get(context, NewString("x")), &geometry->origin.x) &&
    Unwrap(desc->Get(context, NewString("y")), &geometry->origin.y) &&
    Unwrap(desc->Get(context, NewString("width")), &geometry->size.w) &&
    Unwrap(desc->Get(context, NewString("height")), &geometry->size.h)
  );
}


inline bool TryCast(const wlc_size* size, Local<Object>* output) {
  if (!size) return false;

  auto isolate = Isolate::GetCurrent();
  auto context = isolate->GetCurrentContext();

  *output = Object::New(isolate);
  return (
    (*output)->Set(context, NewString("width"), Integer::New(isolate, size->w)).IsJust() &&
    (*output)->Set(context, NewString("height"), Integer::New(isolate, size->h)).IsJust()
  );
}

}

#endif
