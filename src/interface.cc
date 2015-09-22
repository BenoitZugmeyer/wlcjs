#include "wlcjs.h"
#include "output.h"

namespace wlcjs {

#define CALLBACK(NAME, CODE, ...) do {\
  ISOLATE(persistent_interface)\
  HandleScope scope(isolate);\
  Local<Object> interface = persistent_interface.Unwrap();\
  Local<Value> v = interface->Get(S(NAME));\
  if (v->IsFunction()) {\
    CODE;\
    Local<Value> arguments[] = { __VA_ARGS__ };\
    v.As<Function>()->Call(Null(isolate), sizeof(arguments) / sizeof(arguments[0]), arguments);\
  }\
} while (0);\

#define GET_FROM_HANDLE(TYPE, V) TYPE* V ## _js = static_cast<TYPE*>(wlc_handle_get_user_data(V));

SimplePersistent<Object> persistent_interface;

bool output_created(wlc_handle output) {
  // TODO return true or false
  Output* output_js = new Output(output);
  wlc_handle_set_user_data(output, output_js);
  CALLBACK("outputCreated", {}, output_js->GetInstance());
  return true;
}

void output_destroyed(wlc_handle output) {
  GET_FROM_HANDLE(Output, output);
  CALLBACK("outputDestroyed", {}, output_js->GetInstance());
  delete output_js;
}

void output_focus(wlc_handle output, bool focus) {
  GET_FROM_HANDLE(Output, output);
  CALLBACK(
    "outputFocus",
    {},
    output_js->GetInstance(),
    Boolean::New(isolate, focus),
  );
}

void output_resolution(wlc_handle output, const wlc_size* from, const wlc_size* to) {
  // TODO handle from/to
  CALLBACK("outputResolution", {});
}

bool keyboard_key(wlc_handle view, uint32_t time, const wlc_modifiers* modifiers, uint32_t key, wlc_key_state key_state) {
  Local<Object> modifiers_js;
  // TODO return true or false
  CALLBACK(
    "keyboardKey",
    {
      modifiers_js = Object::New(isolate);
      modifiers_js->Set(S("mods"), Number::New(isolate, modifiers->mods));
      modifiers_js->Set(S("leds"), Number::New(isolate, modifiers->leds));
    },
    Undefined(isolate), // TODO view
    Number::New(isolate, time),
    modifiers_js,
    Number::New(isolate, key),
    S(enum_to_string(key_state)),
  );
  return true;
}

bool view_created(wlc_handle view) {
  // TODO return true or false
  CALLBACK(
    "viewCreated",
    {},
    Undefined(isolate), // TODO view
  );
  return true;
}

void view_destroyed(wlc_handle view) {
  CALLBACK(
    "viewDestroyed",
    {},
    Undefined(isolate), // TODO view
  );
}

void view_focus(wlc_handle view, bool focus) {
  CALLBACK(
    "viewFocus",
    {},
    Undefined(isolate), // TODO view
    Boolean::New(isolate, focus),
  );
}

void view_move_to_output(wlc_handle view, wlc_handle from_output, wlc_handle to_output) {
  GET_FROM_HANDLE(Output, from_output);
  GET_FROM_HANDLE(Output, to_output);
  CALLBACK(
    "viewMoveToOutput",
    {},
    Undefined(isolate), // TODO view
    from_output_js->GetInstance(),
    to_output_js->GetInstance(),
  );
}

wlc_interface global_interface = {

  .output = {
    // bool (*created)(wlc_handle output);
    .created = output_created,
    // void (*destroyed)(wlc_handle output);
    .destroyed = output_destroyed,
    // void (*focus)(wlc_handle output, bool focus);
    .focus = output_focus,
    // void (*resolution)(wlc_handle output, const struct wlc_size *from, const struct wlc_size *to);
    .resolution = output_resolution,
  },

  .view = {
    // bool (*created)(wlc_handle view);
    .created = view_created,
    // void (*destroyed)(wlc_handle view);
    .destroyed = view_destroyed,
    // void (*focus)(wlc_handle view, bool focus);
    .focus = view_focus,
    // void (*move_to_output)(wlc_handle view, wlc_handle from_output, wlc_handle to_output);
    .move_to_output = view_move_to_output,

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
  persistent_interface.Reset(value);
  return &global_interface;
}

}
