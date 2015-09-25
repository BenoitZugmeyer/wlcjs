#ifndef _WLCJS_VIEW_H
#define _WLCJS_VIEW_H

#include "managed_object.h"

namespace wlcjs {

class View : public ManagedObject<View> {
 public:
  static constexpr const char* name = "View";
  static void InitPrototype(Isolate* isolate, Local<FunctionTemplate> tpl);
  explicit View(wlc_handle handle) : ManagedObject(handle) {};
};

}

#endif
