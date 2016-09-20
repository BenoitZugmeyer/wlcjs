// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include "./common.h"
#include "./types.h"

namespace wlcjs {

template <class T>
class SimplePersistent : public Persistent<T> {
 public:

  V8_INLINE void Reset() {
    this->isolate = NULL;
    Persistent<T>::Reset();
  }

  V8_INLINE void Reset(Isolate* isolate, const Local<T>& other) {
    this->isolate = isolate;
    Persistent<T>::Reset(isolate, other);
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

inline char* v8string_to_cstring(Local<String> str) {
  String::Utf8Value v(str);
  char* result = new char[v.length() + 1];
  memcpy(result, *v, v.length() + 1);
  return result;
}

}  // namespace wlcjs

#endif  // SRC_UTIL_H_
