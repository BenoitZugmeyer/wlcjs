#ifndef _WLCJS_UTIL_H
#define _WLCJS_UTIL_H

#include "common.h"
#include "types.h"

namespace wlcjs {

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

inline char* v8string_to_cstring(Local<String> str) {
  String::Utf8Value v(str);
  char* result = new char[v.length() + 1];
  memcpy(result, *v, v.length() + 1);
  return result;
}

}

#endif
