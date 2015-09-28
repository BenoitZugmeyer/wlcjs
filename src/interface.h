#ifndef _WLCJS_INTERFACE_H
#define _WLCJS_INTERFACE_H

namespace wlcjs {

wlc_interface* get_wlc_interface(Isolate*, Local<Object>);

}

#endif
