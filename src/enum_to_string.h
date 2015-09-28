#ifndef _WLCJS_ENUM_TO_STRING_H
#define _WLCJS_ENUM_TO_STRING_H

#include "common.h"

namespace wlcjs {

const char* enum_to_string(wlc_log_type);
const char* enum_to_string(wlc_backend_type);
const char* enum_to_string(wlc_key_state);
const char* enum_to_string(wlc_button_state);

}

#endif
