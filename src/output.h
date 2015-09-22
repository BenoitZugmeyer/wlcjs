#ifndef _WLCJS_OUTPUT_H
#define _WLCJS_OUTPUT_H

#include "managed_object.h"

namespace wlcjs {

class Output : public ManagedObject<Output> {
 public:
  static void Init(Local<Object> exports);
  explicit Output(wlc_handle handle) : ManagedObject(handle) {};
};

}

#endif
