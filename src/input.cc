// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#include <xkbcommon/xkbcommon.h>
#include "util.h"
#include "types.h"
#include "state.h"

namespace wlcjs {
namespace Input {

METHOD(GetCurrentKeys) {
  size_t memb;
  Local<Array> result;

  const uint32_t* keys = wlc_keyboard_get_current_keys(&memb);
  if (!TryCast(keys, memb, &result)) return;

  RETURN(result);
}

METHOD(GetKeysymForKey) {
  ISOLATE(info)
  if (~state & STATE_INITIALIZED) {
    THROW(Error, "'init' has to be called before calling 'getKeysymForKey'");
  }

  // TODO(benoitz) modifiers support
  uint32_t key;
  if (!TryCast(info[0], &key)) {
    THROW(TypeError, "getKeysymForKey argument must be a number");
  }

  uint32_t keysym = wlc_keyboard_get_keysym_for_key(key, NULL);

  RETURN(Integer::NewFromUnsigned(isolate, keysym));
}

METHOD(GetKeysymNameForKey) {
  if (~state & STATE_INITIALIZED) {
    THROW(
        Error,
        "'init' has to be called before calling 'getKeysymNameForKey'");
  }

  // TODO(benoitz) modifiers support
  uint32_t key;
  if (!TryCast(info[0], &key)) {
    THROW(TypeError, "getKeysymNameForKey argument must be a number");
  }

  uint32_t keysym = wlc_keyboard_get_keysym_for_key(key, NULL);
  char buffer[100];
  if (xkb_keysym_get_name(keysym, buffer, 100) < 0) {
    THROW(Error, "Invalid keysym");
  }

  RETURN(NewString(buffer));
}

void Export(Local<Object> exports) {
  NODE_SET_METHOD(exports, "getCurrentKeys", GetCurrentKeys);
  NODE_SET_METHOD(exports, "getKeysymForKey", GetKeysymForKey);
  NODE_SET_METHOD(exports, "getKeysymNameForKey", GetKeysymNameForKey);
}

}  // namespace Input
}  // namespace wlcjs
