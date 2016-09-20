// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#include <functional>

#include "./callbacks.h"
#include "./util.h"
#include "./types.h"

namespace wlcjs {
namespace Callbacks {

enum callback_types_t {
  output_created_type,
  output_destroyed_type,
  output_focus_type,
  output_resolution_type,
  output_render_pre_type,
  output_render_post_type,
  output_context_created_type,
  output_context_destroyed_type,
  view_created_type,
  view_destroyed_type,
  view_focus_type,
  view_move_to_output_type,
  view_request_geometry_type,
  view_request_state_type,
  view_request_move_type,
  view_request_resize_type,
  view_render_pre_type,
  view_render_post_type,
  view_properties_updated_type,
  keyboard_key_type,
  pointer_button_type,
  pointer_scroll_type,
  pointer_motion_type,
  touch_type,
  compositor_ready_type,
  compositor_terminate_type,
  input_created_type,
  input_destroyed_type,

  callback_types_count,
};

SimplePersistent<Function> persistent_callbacks[callback_types_count];

bool CallMeMaybe(
    callback_types_t c,
    const unsigned int argc,
    std::function<void (Isolate*, Local<Value>*)> args_formatter,
    bool default_=false) {

  if (persistent_callbacks[c].IsEmpty()) {
    return default_;
  }

  ISOLATE(persistent_callbacks[c]);
  HandleScope scope(isolate);

  Local<Value> argv[argc];
  args_formatter(isolate, argv);

  auto maybe = persistent_callbacks[c].Unwrap()->Call(
      isolate->GetCurrentContext(),
      Null(isolate),
      argc,
      argv);

  return UnwrapOr(maybe, default_);
}

template <class T>
inline Local<Value> TryCastOrNull(Isolate* isolate, T input) {
  Local<Value> output;
  if (!TryCast(input, &output)) {
    output = Null(isolate);
  }
  return output;
}

template <class T>
inline Local<Value> TryCastOrNull(Isolate* isolate, T* input, size_t memb) {
  Local<Array> output;
  if (!TryCast(input, memb, &output)) return Null(isolate);
  return output;
}

bool output_created_cb(wlc_handle output) {
  return CallMeMaybe(
      output_created_type,
      1,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, output);
      },
      true
  );
}

void output_destroyed_cb(wlc_handle output) {
  CallMeMaybe(
      output_destroyed_type,
      1,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, output);
      }
  );
}

void output_focus_cb(wlc_handle output, bool focus) {
  CallMeMaybe(
      output_focus_type,
      2,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, output);
        args[1] = Boolean::New(isolate, focus);
      }
  );
}

void output_resolution_cb(
    wlc_handle output,
    const wlc_size* from,
    const wlc_size* to) {
  CallMeMaybe(
      output_resolution_type,
      3,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, output);
        args[1] = TryCastOrNull(isolate, from);
        args[2] = TryCastOrNull(isolate, to);
      }
  );
}

bool view_created_cb(wlc_handle view) {
  return CallMeMaybe(
      view_created_type,
      1,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
      },
      true
  );
}

void view_destroyed_cb(wlc_handle view) {
  CallMeMaybe(
      view_destroyed_type,
      1,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
      }
  );
}

void view_focus_cb(wlc_handle view, bool focus) {
  CallMeMaybe(
      view_focus_type,
      2,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
        args[1] = Boolean::New(isolate, focus);
      }
  );
}

void view_move_to_output_cb(
    wlc_handle view,
    wlc_handle from_output,
    wlc_handle to_output) {
  CallMeMaybe(
      view_move_to_output_type,
      3,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
        args[1] = Number::New(isolate, from_output);
        args[2] = Number::New(isolate, to_output);
      }
  );
}

void view_request_geometry_cb(
    wlc_handle view,
    const struct wlc_geometry* geometry) {
  CallMeMaybe(
      view_request_geometry_type,
      2,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
        args[1] = TryCastOrNull(isolate, geometry);
      }
  );
}

void view_request_move_cb(
    wlc_handle view,
    const wlc_point *point) {
  CallMeMaybe(
      view_request_move_type,
      2,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
        args[1] = TryCastOrNull(isolate, point);
      }
  );
}

void view_request_resize_cb(
    wlc_handle view,
    uint32_t edges,
    const wlc_point *point) {
  CallMeMaybe(
      view_request_resize_type,
      3,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
        args[1] = TryCastOrNull(isolate, edges);
        args[2] = TryCastOrNull(isolate, point);
      }
  );
}

bool keyboard_key_cb(
    wlc_handle view,
    uint32_t time,
    const wlc_modifiers* modifiers,
    uint32_t key,
    wlc_key_state key_state) {
  return CallMeMaybe(
      keyboard_key_type,
      5,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
        args[1] = Number::New(isolate, time);
        args[2] = TryCastOrNull(isolate, modifiers);
        args[3] = Number::New(isolate, key);
        args[4] = Number::New(isolate, key_state);
      }
    );
}

bool pointer_button_cb(
    wlc_handle view,
    uint32_t time,
    const struct wlc_modifiers* modifiers,
    uint32_t button,
    enum wlc_button_state state,
    const struct wlc_point* origin) {

  return CallMeMaybe(
      pointer_button_type,
      6,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
        args[1] = Number::New(isolate, time);
        args[2] = TryCastOrNull(isolate, modifiers);
        args[3] = Number::New(isolate, button);
        args[4] = Number::New(isolate, state);
        args[5] = TryCastOrNull(isolate, origin);
      }
    );
}

bool pointer_scroll_cb(wlc_handle view, uint32_t time,
    const struct wlc_modifiers* modifiers, uint8_t axis_bits,
    double amount[2]) {

  return CallMeMaybe(
      pointer_scroll_type,
      5,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
        args[1] = Number::New(isolate, time);
        args[2] = TryCastOrNull(isolate, modifiers);
        args[3] = Number::New(isolate, axis_bits);
        args[4] = TryCastOrNull(isolate, amount, 2);
      }
  );
}

bool pointer_motion_cb(wlc_handle view, uint32_t time,
    const struct wlc_point* origin) {

  return CallMeMaybe(
      pointer_motion_type,
      3,
      [=](Isolate* isolate, Local<Value>* args) {
        args[0] = Number::New(isolate, view);
        args[1] = Number::New(isolate, time);
        args[2] = TryCastOrNull(isolate, origin);
      }
  );
}

void set_callback(
    callback_types_t c,
    const FunctionCallbackInfo<Value>& info,
    std::function<void (bool)> setter) {
  Local<Function> callback;

  if (info[0]->IsNull() || info[0]->IsUndefined()) {
    setter(false);
    persistent_callbacks[c].Reset();
  }
  else if (TryCast(info[0], &callback)) {
    setter(true);
    persistent_callbacks[c].Reset(info.GetIsolate(), callback);
  }
  else {
    THROW(TypeError, "Argument should be a function");
  }
}


void Export(Local<Object> exports) {

#define __DEFINE_SETTER(name, js_name) \
  NODE_SET_METHOD(exports, js_name, [](const FunctionCallbackInfo<Value>& info) { \
      set_callback(name ## _type, info, [](bool should_set) {\
        wlc_set_ ## name ## _cb(should_set ? name ## _cb : NULL); \
      }); \
  });

  __DEFINE_SETTER(output_created, "setOutputCreatedCb");
  __DEFINE_SETTER(output_destroyed, "setOutputDestroyedCb");

  __DEFINE_SETTER(output_focus, "setOutputFocusCb");
  __DEFINE_SETTER(output_resolution, "setOutputResolutionCb");
  /* __DEFINE_SETTER(output_render_pre, "setOutputRenderPreCb"); */
  /* __DEFINE_SETTER(output_render_post, "setOutputRenderPostCb"); */
  /* __DEFINE_SETTER(output_context_created, "setOutputContextCreatedCb"); */
  /* __DEFINE_SETTER(output_context_destroyed, "setOutputContextDestroyedCb"); */
  __DEFINE_SETTER(view_created, "setViewCreatedCb");
  __DEFINE_SETTER(view_destroyed, "setViewDestroyedCb");
  __DEFINE_SETTER(view_focus, "setViewFocusCb");
  __DEFINE_SETTER(view_move_to_output, "setViewMoveToOutputCb");
  __DEFINE_SETTER(view_request_geometry, "setViewRequestGeometryCb");
  /* __DEFINE_SETTER(view_request_state, "setViewRequestStateCb"); */
  __DEFINE_SETTER(view_request_move, "setViewRequestMoveCb");
  __DEFINE_SETTER(view_request_resize, "setViewRequestResizeCb");
  /* __DEFINE_SETTER(view_render_pre, "setViewRenderPreCb"); */
  /* __DEFINE_SETTER(view_render_post, "setViewRenderPostCb"); */
  /* __DEFINE_SETTER(view_properties_updated, "setViewPropertiesUpdatedCb"); */
  __DEFINE_SETTER(keyboard_key, "setKeyboardKeyCb");
  __DEFINE_SETTER(pointer_button, "setPointerButtonCb");
  __DEFINE_SETTER(pointer_scroll, "setPointerScrollCb");
  __DEFINE_SETTER(pointer_motion, "setPointerMotionCb");
  /* __DEFINE_SETTER(touch, "setToutchCb"); */
  /* __DEFINE_SETTER(compositor_ready, "setCompositorReadyCb"); */
  /* __DEFINE_SETTER(compositor_terminate, "setCompositorTerminateCb"); */
  /* __DEFINE_SETTER(input_created, "setInputCreatedCb"); */
  /* __DEFINE_SETTER(input_destroyed, "setInputDestroyedCb"); */

#undef __DEFINE_SETTER
}


}  // namespace Callbacks
}  // namespace wlcjs
