#include "wlcjs.h"

namespace wlcjs {

#define CALLBACK(NAME, CODE, ...) \
  Isolate* isolate = persistent_interface.GetIsolate();\
  HandleScope scope(isolate);\
  Local<Object> interface = Local<Object>::New(isolate, persistent_interface);\
  Local<Value> v = interface->Get(S(NAME));\
  if (v->IsFunction()) {\
    CODE\
    Local<Value> arguments[] = { __VA_ARGS__ };\
    v.As<Function>()->Call(Null(isolate), sizeof(arguments) / sizeof(arguments[0]), arguments);\
  }

SimplePersistent<Object> persistent_interface;

bool output_created(wlc_handle output) {
  CALLBACK("outputCreated", {});
  return true;
}

void output_resolution(wlc_handle output, const wlc_size* from, const wlc_size* to) {
  CALLBACK("resolutionChanged", {});
}

bool keyboard_key(wlc_handle view, uint32_t time, const wlc_modifiers*, uint32_t key, wlc_key_state key_state) {
  CALLBACK(
    (key_state == WLC_KEY_STATE_PRESSED ? "keyPressed" : "keyReleased"),
    {},
    Number::New(isolate, key),
    Number::New(isolate, time),
  );
  return true;
}

wlc_interface global_interface = {

  .output = {
    // bool (*created)(wlc_handle output);
    .created = output_created,
    // void (*destroyed)(wlc_handle output);
    .destroyed = NULL,
    // void (*focus)(wlc_handle output, bool focus);
    .focus = NULL,
    // void (*resolution)(wlc_handle output, const struct wlc_size *from, const struct wlc_size *to);
    .resolution = output_resolution,
  },

  .view = {
    // bool (*created)(wlc_handle view);
    .created = NULL,
    // void (*destroyed)(wlc_handle view);
    .destroyed = NULL,
    // void (*focus)(wlc_handle view, bool focus);
    .focus = NULL,
    // void (*move_to_output)(wlc_handle view, wlc_handle from_output, wlc_handle to_output);
    .move_to_output = NULL,

    .request = {
      // void (*geometry)(wlc_handle view, const struct wlc_geometry*);
      .geometry = NULL,
      // void (*state)(wlc_handle view, enum wlc_view_state_bit, bool toggle);
      .state = NULL,
      // void (*move)(wlc_handle view, const struct wlc_origin *origin);
      .move = NULL,
      // void (*resize)(wlc_handle view, uint32_t edges, const struct wlc_origin *origin);
      .resize = NULL,
    },
  },

  .keyboard = {
    // bool (*key)(wlc_handle view, uint32_t time, const struct wlc_modifiers*, uint32_t key, enum wlc_key_state);
    .key = keyboard_key,
  },

  .pointer = {
    // bool (*button)(wlc_handle view, uint32_t time, const struct wlc_modifiers*, uint32_t button, enum wlc_button_state, const struct wlc_origin*);
    .button = NULL,
    // bool (*scroll)(wlc_handle view, uint32_t time, const struct wlc_modifiers*, uint8_t axis_bits, double amount[2]);
    .scroll = NULL,
    // bool (*motion)(wlc_handle view, uint32_t time, const struct wlc_origin*);
    .motion = NULL,
  },

  .touch = {
    // bool (*touch)(wlc_handle view, uint32_t time, const struct wlc_modifiers*, enum wlc_touch_type, int32_t slot, const struct wlc_origin*);
    .touch = NULL,
  },

  .compositor = {
    // void (*ready)(void);
    .ready = NULL,
  },

  .input = {
    // bool (*created)(struct libinput_device *device);
    .created = NULL,
    // void (*destroyed)(struct libinput_device *device);
    .destroyed = NULL,
  },

};

wlc_interface* get_wlc_interface(Local<Object> value) {
  if (!is_initalized()) {
    persistent_interface.Reset(value);
  }
  return &global_interface;
}

bool is_initalized() {
  return !persistent_interface.IsEmpty();
}

}
