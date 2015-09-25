#ifndef _WLCJS_OUTPUT_H
#define _WLCJS_OUTPUT_H

#include "managed_object.h"

namespace wlcjs {

class Output : public ManagedObject<Output> {
 public:
  static constexpr const char* name = "Output";
  static void InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl);
  explicit Output(wlc_handle handle) : ManagedObject(handle) {};
};

}

#endif
