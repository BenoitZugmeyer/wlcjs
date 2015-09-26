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

_DEFINE(Object)
_DEFINE(Array)
_DEFINE(String)
_DEFINE(Number)
_DEFINE(Function)

template <class T>
inline Local<T> GetLocalChecked(Local<Value> value, const char* name, va_list args) {
  if (!IsType<T>(value)) {
    char buffer[256];
    vsnprintf(buffer, 256, name, args);
    ThrowException(Exception::TypeError, "%s should be a %s", buffer, GetTypeName<T>());
    return Local<T>();
  }
  return value.As<T>();
}

template <class T>
inline Local<T> GetLocalChecked(Local<Value> value, const char* name, ...)
  MK_VARIADIC_N(GetLocalChecked<T>, name, value)

template <class T>
inline Local<T> GetLocalChecked(MaybeLocal<Value> maybe, const char* name, va_list args) {
  if (maybe.IsEmpty()) return Local<T>();
  Local<Value> value = maybe.ToLocalChecked();
  return GetLocalChecked<T>(value, name, args);
}

template <class T>
inline Local<T> GetLocalChecked(MaybeLocal<T> maybe) {
  if (maybe.IsEmpty()) return Local<T>();
  return maybe.ToLocalChecked();
}

template <class T>
inline Local<T> GetLocalChecked(MaybeLocal<Value> maybe, const char* name, ...)
  MK_VARIADIC_N(GetLocalChecked<T>, name, maybe)

#define GET_LOCAL_AS(TYPE, NAME, VALUE, ...) \
  Local<TYPE> NAME = GetLocalChecked<TYPE>(VALUE, ##__VA_ARGS__);\
  if (NAME.IsEmpty()) return;

#define CHECK_OR(VALUE, OR) if ((VALUE).IsNothing()) { OR; }

#define UNWRAP_OR(NAME, VALUE, OR) \
  auto maybe_ ## NAME = VALUE; \
  if (maybe_ ## NAME .IsNothing()) { OR; } \
  auto NAME = maybe_ ## NAME .FromJust();

}

#endif
