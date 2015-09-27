#include "wlcjs.h"
#include "output.h"
#include "view.h"

namespace wlcjs {

SimplePersistent<Object> persistent_interface;

#define MK_SCOPE\
  ISOLATE(persistent_interface); \
  HandleScope scope(isolate);

MaybeLocal<Value> CallCallback(const char* name, int argc, Local<Value> argv[]) {
  ISOLATE(persistent_interface);
  Local<Object> interface = persistent_interface.Unwrap();
  Local<Value> v;

  if (!Unwrap(interface->Get(isolate->GetCurrentContext(), NewString(name)), &v)) {
    return MaybeLocal<Value>();
  }

  if (v->IsFunction()) {
    return v.As<Function>()->Call(isolate->GetCurrentContext(), Null(isolate), argc, argv);
  }
  return Undefined(isolate);
}

bool output_created(wlc_handle output) {
  // TODO return true or false
  MK_SCOPE
  Output* output_js;

  if (!Unwrap(ManagedObject<Output>::Create(isolate, output), &output_js)) {
    return false; // TODO maybe write some kind of log?
  }

  wlc_handle_set_user_data(output, output_js);
  Local<Value> argv[] = { output_js->GetInstance() };
  CallCallback("outputCreated", 1, argv);
  return true;
}

void output_destroyed(wlc_handle output) {
  MK_SCOPE
  auto output_js = Output::FromWLCHandle(output);
  Local<Value> argv[] = { output_js->GetInstance() };
  CallCallback("outputDestroyed", 1, argv);
  delete output_js;
}

void output_focus(wlc_handle output, bool focus) {
  MK_SCOPE
  Local<Value> argv[] = {
    Output::FromWLCHandle(output)->GetInstance(),
    Boolean::New(isolate, focus),
  };
  CallCallback("outputFocus", 2, argv);
}

void output_resolution(wlc_handle output, const wlc_size* from, const wlc_size* to) {
  MK_SCOPE
  // TODO handle from/to
  CallCallback("outputResolution", 0, NULL);
}

bool keyboard_key(wlc_handle view, uint32_t time, const wlc_modifiers* modifiers, uint32_t key, wlc_key_state key_state) {
  MK_SCOPE
  Local<Object> modifiers_js;
  Local<Value> view_or_undefined;

  if (view) {
    view_or_undefined = Output::FromWLCHandle(view)->GetInstance();
  }
  else {
    view_or_undefined = Undefined(isolate);
  }

  modifiers_js = Object::New(isolate);
  modifiers_js->Set(isolate->GetCurrentContext(), NewString("mods"), Number::New(isolate, modifiers->mods));
  modifiers_js->Set(isolate->GetCurrentContext(), NewString("leds"), Number::New(isolate, modifiers->leds));

  Local<Value> argv[] = {
    view_or_undefined,
    Number::New(isolate, time),
    modifiers_js,
    Number::New(isolate, key),
    NewString(enum_to_string(key_state)),
  };

  // TODO return true or false
  CallCallback("keyboardKey", 5, argv);
  return true;
}

bool view_created(wlc_handle view) {
  // TODO return true or false
  MK_SCOPE
  View* view_js;
  if (!Unwrap(View::Create(isolate, view), &view_js)) {
    return false; // TODO maybe write some kind of log?
  }
  wlc_handle_set_user_data(view, view_js);

  Local<Value> argv[] = {
    view_js->GetInstance(),
  };

  CallCallback("viewCreated", 1, argv);
  return true;
}

void view_destroyed(wlc_handle view) {
  MK_SCOPE

  auto view_js = View::FromWLCHandle(view);

  Local<Value> argv[] = {
    view_js->GetInstance(),
  };

  CallCallback("viewDestroyed", 1, argv);

  delete view_js;
}

void view_focus(wlc_handle view, bool focus) {
  MK_SCOPE

  Local<Value> argv[] = {
    View::FromWLCHandle(view)->GetInstance(),
    Boolean::New(isolate, focus),
  };
  CallCallback("viewFocus", 2, argv);
}

void view_move_to_output(wlc_handle view, wlc_handle from_output, wlc_handle to_output) {
  MK_SCOPE

  Local<Value> argv[] = {
    View::FromWLCHandle(view)->GetInstance(),
    Output::FromWLCHandle(from_output)->GetInstance(),
    Output::FromWLCHandle(to_output)->GetInstance(),
  };
  CallCallback("viewMoveToOutput", 3, argv);
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

wlc_interface* get_wlc_interface(Isolate* isolate, Local<Object> value) {
  persistent_interface.Reset(isolate, value);
  return &global_interface;
}

}
