// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#ifndef SRC_TYPES_H_
#define SRC_TYPES_H_

#include "./common.h"

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
_DEFINE(Boolean)
_DEFINE(Array)
_DEFINE(String)
_DEFINE(Number)
_DEFINE(Function)

template <class T>
inline bool Unwrap(Maybe<T> value, T* dest) {
  if (value.IsNothing()) return false;
  *dest = value.FromJust();
  return true;
}

template <class T>
inline bool TryCast(Local<Value> value, Local<T>* dest) {
  if (!IsType<T>(value)) return false;
  *dest = value.As<T>();
  return true;
}

inline bool TryCast(Local<Value> value, uint32_t* dest) {
  if (!IsType<Number>(value)) return false;
  return Unwrap(
      value->Uint32Value(Isolate::GetCurrent()->GetCurrentContext()),
      dest);
}

inline bool TryCast(Local<Value> value, int32_t* dest) {
  if (!IsType<Number>(value)) return false;
  return Unwrap(
      value->Int32Value(Isolate::GetCurrent()->GetCurrentContext()),
      dest);
}

inline bool TryCast(Local<Value> value, bool* dest) {
  if (!IsType<Boolean>(value)) return false;
  return Unwrap(
      value->BooleanValue(Isolate::GetCurrent()->GetCurrentContext()),
      dest);
}

template <class INPUT, class OUTPUT>
inline bool Unwrap(MaybeLocal<INPUT> maybe, OUTPUT* dest) {
  if (maybe.IsEmpty()) return false;
  return TryCast(maybe.ToLocalChecked(), dest);
}

inline bool TryCast(Local<Value> desc, wlc_geometry* geometry) {
  Local<Object> desc_object;
  if (!TryCast(desc, &desc_object)) return false;

  auto context = Isolate::GetCurrent()->GetCurrentContext();

  return (
    Unwrap(desc_object->Get(context, NewString("x")), &geometry->origin.x) &&
    Unwrap(desc_object->Get(context, NewString("y")), &geometry->origin.y) &&
    Unwrap(desc_object->Get(context, NewString("width")), &geometry->size.w) &&
    Unwrap(desc_object->Get(context, NewString("height")), &geometry->size.h));
}

inline bool TryCast(const wlc_geometry* geometry, Local<Object>* output) {
  auto isolate = Isolate::GetCurrent();
  auto context = isolate->GetCurrentContext();

  *output = Object::New(isolate);
  return (
    (*output)->Set(
      context,
      NewString("x"),
      Integer::New(isolate, geometry->origin.x)).IsJust() &&

    (*output)->Set(
      context,
      NewString("y"),
      Integer::New(isolate, geometry->origin.y)).IsJust() &&

    (*output)->Set(
      context,
      NewString("width"),
      Integer::New(isolate, geometry->size.w)).IsJust() &&

    (*output)->Set(
      context,
      NewString("height"),
      Integer::New(isolate, geometry->size.h)).IsJust());
}

inline bool TryCast(Local<Value> value, wlc_handle* dest) {
  uint32_t dest_small;
  if (!TryCast(value, &dest_small) || !dest_small) return false;
  *dest = dest_small;
  return true;
}

inline bool TryCast(wlc_handle value, Local<Value>* dest) {
  auto isolate = Isolate::GetCurrent();
  *dest = Number::New(isolate, value);
  return true;
}

inline bool TryCast(const wlc_size* size, Local<Object>* output) {
  if (!size) return false;

  auto isolate = Isolate::GetCurrent();
  auto context = isolate->GetCurrentContext();

  *output = Object::New(isolate);
  return (
    (*output)->Set(
      context,
      NewString("width"),
      Integer::New(isolate, size->w)).IsJust() &&

    (*output)->Set(
      context,
      NewString("height"),
      Integer::New(isolate, size->h)).IsJust());
}

inline bool TryCast(const wlc_modifiers* modifiers, Local<Object>* output) {
  if (!modifiers) return false;

  auto isolate = Isolate::GetCurrent();
  auto context = isolate->GetCurrentContext();

  *output = Object::New(isolate);
  return (
    (*output)->Set(
      context,
      NewString("mods"),
      Integer::New(isolate, modifiers->mods)).IsJust() &&

    (*output)->Set(
      context,
      NewString("height"),
      Integer::New(isolate, modifiers->leds)).IsJust());
}

inline bool TryCast(const wlc_point* point, Local<Object>* output) {
  if (!point) return false;

  auto isolate = Isolate::GetCurrent();
  auto context = isolate->GetCurrentContext();

  *output = Object::New(isolate);
  return (
    (*output)->Set(
      context,
      NewString("x"),
      Integer::New(isolate, point->x)).IsJust() &&

    (*output)->Set(
      context,
      NewString("y"),
      Integer::New(isolate, point->y)).IsJust());
}

template <class T>
inline bool TryCast(const T* arr, size_t memb, Local<Array>* output) {
  auto isolate = Isolate::GetCurrent();
  auto context = isolate->GetCurrentContext();

  *output = Array::New(isolate, memb);

  Local<Value> value;
  for (size_t i = 0; i < memb; i += 1) {
    if (!TryCast(arr[i], &value)) return false;
    if ((*output)->Set(context, i, value).IsNothing()) return false;
  }

  return true;
}

template <class T>
inline T UnwrapOr(MaybeLocal<Value> value, T default_) {
  T result;
  if (Unwrap(value, &result)) return result;
  return default_;
}


}  // namespace wlcjs

#endif  // SRC_TYPES_H_
