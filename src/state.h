// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#ifndef SRC_STATE_H_
#define SRC_STATE_H_

namespace wlcjs {

enum state_t {
  STATE_UNINITIALIZED = 0x1,
  STATE_INITIALIZED = 0x2,
  STATE_RUNNING = 0x4,
  STATE_TERMINATED = 0x8,
};

extern int state;

}  // namespace wlcjs

#endif  // SRC_STATE_H_
