#ifndef _WLCJS_VIEW_H
#define _WLCJS_VIEW_H

#include "managed_object.h"

namespace wlcjs {

class View : public ManagedObject<View> {
 public:
  static void Init(Local<Object> exports);
  explicit View(wlc_handle handle) : ManagedObject(handle) {};
};

}

#endif
