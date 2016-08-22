// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#ifndef SRC_CALLBACKS_H_
#define SRC_CALLBACKS_H_

#include "./types.h"

namespace wlcjs {
namespace Callbacks {

void init();

void Export(Local<Object> exports);

}
}

#endif  // SRC_CALLBACKS_H_
