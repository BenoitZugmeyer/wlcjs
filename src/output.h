#ifndef _WLCJS_OUTPUT_H
#define _WLCJS_OUTPUT_H

#include "wlcjs.h"

namespace wlcjs {

class Output {
 public:
  static void Init(Local<Object> exports);
  explicit Output(wlc_handle);
  ~Output();

  Local<Object> instance();
  wlc_handle output();

 private:
  static SimplePersistent<Function> constructor;
  wlc_handle output_;
  SimplePersistent<Object> instance_;
};

}

#endif
