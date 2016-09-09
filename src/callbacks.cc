// Copyright (c) 2016 Benoît Zugmeyer
// Use of this source code is governed by a MIT-style license that can be found
// in the LICENSE file.

#include "./callbacks.h"
#include "./util.h"
#include "./types.h"

namespace wlcjs {
namespace Callbacks {

SimplePersistent<Object> persistent_callbacks;

#define MK_SCOPE\
  ISOLATE(persistent_callbacks); \
  HandleScope scope(isolate);

MaybeLocal<Value> CallMeMaybe(const char* name, int argc, Local<Value> argv[]) {
  ISOLATE(persistent_callbacks);
  Local<Object> callbacks = persistent_callbacks.Unwrap();
  Local<Function> v;

  if (!Unwrap(
        callbacks->Get(isolate->GetCurrentContext(), NewString(name)),
        &v)) {
    return MaybeLocal<Value>();
  }

  return v.As<Function>()->Call(
      isolate->GetCurrentContext(),
      Null(isolate),
      argc,
      argv);
}

bool output_created_cb(wlc_handle output) {
  MK_SCOPE
  Local<Value> argv[] = {
    Number::New(isolate, output),
  };
  return UnwrapOr(CallMeMaybe("outputCreated", 1, argv), true);
}

void output_destroyed_cb(wlc_handle output) {
  MK_SCOPE
  Local<Value> argv[] = {
    Number::New(isolate, output),
  };
  CallMeMaybe("outputDestroyed", 1, argv);
}

void output_focus_cb(wlc_handle output, bool focus) {
  MK_SCOPE
  Local<Value> argv[] = {
    Number::New(isolate, output),
    Boolean::New(isolate, focus),
  };
  CallMeMaybe("outputFocus", 2, argv);
}

void output_resolution_cb(
    wlc_handle output,
    const wlc_size* from,
    const wlc_size* to) {
  MK_SCOPE
  Local<Object> from_js;
  Local<Object> to_js;
  if (!TryCast(from, &from_js)) return;
  if (!TryCast(to, &to_js)) return;

  Local<Value> argv[] = {
    Number::New(isolate, output),
    from_js,
    to_js,
  };
  CallMeMaybe("outputResolution", 3, argv);
}

bool view_created_cb(wlc_handle view) {
  MK_SCOPE
  Local<Value> argv[] = {
    Number::New(isolate, view),
  };
  return UnwrapOr(CallMeMaybe("viewCreated", 1, argv), true);
}

void view_destroyed_cb(wlc_handle view) {
  MK_SCOPE
  Local<Value> argv[] = {
    Number::New(isolate, view),
  };
  CallMeMaybe("viewDestroyed", 1, argv);
}

void view_focus_cb(wlc_handle view, bool focus) {
  MK_SCOPE
  Local<Value> argv[] = {
    Number::New(isolate, view),
    Boolean::New(isolate, focus),
  };
  CallMeMaybe("viewFocus", 2, argv);
}

void view_move_to_output_cb(
    wlc_handle view,
    wlc_handle from_output,
    wlc_handle to_output) {
  MK_SCOPE
  Local<Value> argv[] = {
    Number::New(isolate, view),
    Number::New(isolate, from_output),
    Number::New(isolate, to_output),
  };
  CallMeMaybe("viewMoveToOutput", 3, argv);
}

void view_request_move_cb(
    wlc_handle view,
    const wlc_point *point) {
  MK_SCOPE

  Local<Object> point_js;
  if (!TryCast(point, &point_js)) return;

  Local<Value> argv[] = {
    Number::New(isolate, view),
    point_js,
  };
  CallMeMaybe("viewRequestMove", 2, argv);
}

void view_request_resize_cb(
    wlc_handle view,
    uint32_t edges,
    const wlc_point *point) {
  MK_SCOPE

  Local<Integer> edges_js;
  Local<Object> point_js;
  if (!TryCast(edges, &edges_js)) return;
  if (!TryCast(point, &point_js)) return;

  Local<Value> argv[] = {
    Number::New(isolate, view),
    edges_js,
    point_js,
  };
  CallMeMaybe("viewRequestResize", 3, argv);
}

bool keyboard_key_cb(
    wlc_handle view,
    uint32_t time,
    const wlc_modifiers* modifiers,
    uint32_t key,
    wlc_key_state key_state) {
  MK_SCOPE
  Local<Object> modifiers_js;
  if (!TryCast(modifiers, &modifiers_js)) return false;

  Local<Value> argv[] = {
    Number::New(isolate, view),
    Number::New(isolate, time),
    modifiers_js,
    Number::New(isolate, key),
    Number::New(isolate, key_state),
  };
  return UnwrapOr(CallMeMaybe("keyboardKey", 5, argv), false);
}

bool pointer_button_cb(
    wlc_handle view,
    uint32_t time,
    const struct wlc_modifiers* modifiers,
    uint32_t button,
    enum wlc_button_state state,
    const struct wlc_point* origin) {

  MK_SCOPE
  Local<Object> modifiers_js;
  Local<Object> origin_js;
  if (!TryCast(modifiers, &modifiers_js)) return false;
  if (!TryCast(origin, &origin_js)) return false;

  Local<Value> argv[] = {
    Number::New(isolate, view),
    Number::New(isolate, time),
    modifiers_js,
    Number::New(isolate, button),
    Number::New(isolate, state),
    origin_js,
  };
  return UnwrapOr(CallMeMaybe("pointerButton", 6, argv), false);
}

bool pointer_scroll_cb(wlc_handle view, uint32_t time,
    const struct wlc_modifiers* modifiers, uint8_t axis_bits,
    double amount[2]) {

  MK_SCOPE
  auto context = isolate->GetCurrentContext();
  Local<Object> modifiers_js;
  if (!TryCast(modifiers, &modifiers_js)) return false;

  Local<Array> amount_js = Array::New(isolate, 2);
  if (amount_js->Set(context, 0, Number::New(isolate, amount[0])).IsNothing() ||
      amount_js->Set(context, 1, Number::New(isolate, amount[1])).IsNothing()) {
    return false;
  }

  Local<Value> argv[] = {
    Number::New(isolate, view),
    Number::New(isolate, time),
    modifiers_js,
    Number::New(isolate, axis_bits),
    amount_js,
  };
  return UnwrapOr(CallMeMaybe("pointerScroll", 5, argv), false);
}

bool pointer_motion_cb(wlc_handle view, uint32_t time,
    const struct wlc_point* origin) {

  MK_SCOPE
  Local<Object> origin_js;
  if (!TryCast(origin, &origin_js)) return false;

  Local<Value> argv[] = {
    Number::New(isolate, view),
    Number::New(isolate, time),
    origin_js,
  };
  return UnwrapOr(CallMeMaybe("pointerMotion", 3, argv), false);
}


void Export(Local<Object> exports) {
  Isolate* isolate = Isolate::GetCurrent();
  persistent_callbacks.Reset(isolate, exports);
}


void init() {
  wlc_set_output_created_cb(output_created_cb);
  wlc_set_output_destroyed_cb(output_destroyed_cb);
  wlc_set_output_focus_cb(output_focus_cb);
  wlc_set_output_resolution_cb(output_resolution_cb);
  /* wlc_set_output_render_pre_cb(output_render_pre_cb); */
  /* wlc_set_output_render_post_cb(output_render_post_cb); */
  /* wlc_set_output_context_created_cb(output_context_created_cb); */
  /* wlc_set_output_context_destroyed_cb(output_context_destroyed_cb); */
  wlc_set_view_created_cb(view_created_cb);
  wlc_set_view_destroyed_cb(view_destroyed_cb);
  wlc_set_view_focus_cb(view_focus_cb);
  wlc_set_view_move_to_output_cb(view_move_to_output_cb);
  /* wlc_set_view_request_geometry_cb(view_request_geometry_cb); */
  /* wlc_set_view_request_state_cb(view_request_state_cb); */
  wlc_set_view_request_move_cb(view_request_move_cb);
  wlc_set_view_request_resize_cb(view_request_resize_cb);
  /* wlc_set_view_render_pre_cb(view_render_pre_cb); */
  /* wlc_set_view_render_post_cb(view_render_post_cb); */
  /* wlc_set_view_properties_updated_cb(view_properties_updated_cb); */
  wlc_set_keyboard_key_cb(keyboard_key_cb);
  wlc_set_pointer_button_cb(pointer_button_cb);
  wlc_set_pointer_scroll_cb(pointer_scroll_cb);
  wlc_set_pointer_motion_cb(pointer_motion_cb);
  /* wlc_set_touch_cb(touch_cb); */
  /* wlc_set_compositor_ready_cb(compositor_ready_cb); */
  /* wlc_set_compositor_terminate_cb(compositor_terminate_cb); */
  /* wlc_set_input_created_cb(input_created_cb); */
  /* wlc_set_input_destroyed_cb(input_destroyed_cb); */
}


}  // namespace Callbacks
}  // namespace wlcjs
