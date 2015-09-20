#ifndef _WLCJS_H
#define _WLCJS_H

extern "C" {
#include <wlc/wlc.h>
}

#include <node.h>
#include "uv.h"

using v8::Isolate;
using v8::HandleScope;
using v8::Object;
using v8::Local;
using v8::Value;
using v8::String;
using v8::Function;
using v8::Persistent;
using v8::Number;
using v8::FunctionCallbackInfo;
using v8::Exception;
using v8::Array;

#define S(C) String::NewFromUtf8(isolate, C)

#define THROW(T, E) {\
    isolate->ThrowException(Exception::T(S(E)));\
    return;\
}

wlc_interface* get_wlc_interface(Local<Object>);
bool is_initalized();

#endif
