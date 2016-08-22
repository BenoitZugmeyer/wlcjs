#ifndef _WLCJS_STATE_H
#define _WLCJS_STATE_H

namespace wlcjs {

enum state_t {
  STATE_UNINITIALIZED = 0x1,
  STATE_INITIALIZED = 0x2,
  STATE_RUNNING = 0x4,
  STATE_TERMINATED = 0x8,
};

extern int state;

}

#endif
