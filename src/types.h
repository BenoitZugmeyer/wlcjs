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
} ;

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

}

#endif
