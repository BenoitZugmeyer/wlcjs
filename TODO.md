TODO
====

Data structures
---------------

- [x] `enum wlc_log_type`
- [x] `enum wlc_backend_type`
- [x] `enum wlc_event_bit`
- [x] `enum wlc_view_state_bit`
- [x] `enum wlc_view_type_bit`
- [x] `enum wlc_view_property_update_bit`
- [x] `enum wlc_resize_edge`
- [x] `enum wlc_modifier_bit`
- [x] `enum wlc_led_bit`
- [x] `enum wlc_key_state`
- [x] `enum wlc_button_state`
- [x] `enum wlc_scroll_axis_bit`
- [x] `enum wlc_touch_type`
- [ ] `struct wlc_modifiers`

Callbacks API
-------------

- [x] `void wlc_set_output_created_cb(bool (*cb)(wlc_handle output))`
- [x] `void wlc_set_output_destroyed_cb(void (*cb)(wlc_handle output))`
- [x] `void wlc_set_output_focus_cb(void (*cb)(wlc_handle output, bool focus))`
- [x] `void wlc_set_output_resolution_cb(void (*cb)(wlc_handle output, const struct wlc_size *from, const struct wlc_size *to))`
- [ ] `void wlc_set_output_render_pre_cb(void (*cb)(wlc_handle output))`
- [ ] `void wlc_set_output_render_post_cb(void (*cb)(wlc_handle output))`
- [ ] `void wlc_set_output_context_created_cb(void (*cb)(wlc_handle output))`
- [ ] `void wlc_set_output_context_destroyed_cb(void (*cb)(wlc_handle output))`
- [x] `void wlc_set_view_created_cb(bool (*cb)(wlc_handle view))`
- [x] `void wlc_set_view_destroyed_cb(void (*cb)(wlc_handle view))`
- [x] `void wlc_set_view_focus_cb(void (*cb)(wlc_handle view, bool focus))`
- [x] `void wlc_set_view_move_to_output_cb(void (*cb)(wlc_handle view, wlc_handle from_output, wlc_handle to_output))`
- [x] `void wlc_set_view_request_geometry_cb(void (*cb)(wlc_handle view, const struct wlc_geometry*))`
- [ ] `void wlc_set_view_request_state_cb(void (*cb)(wlc_handle view, enum wlc_view_state_bit, bool toggle))`
- [x] `void wlc_set_view_request_move_cb(void (*cb)(wlc_handle view, const struct wlc_point*))`
- [x] `void wlc_set_view_request_resize_cb(void (*cb)(wlc_handle view, uint32_t edges, const struct wlc_point*))`
- [ ] `void wlc_set_view_render_pre_cb(void (*cb)(wlc_handle view))`
- [ ] `void wlc_set_view_render_post_cb(void (*cb)(wlc_handle view))`
- [ ] `void wlc_set_view_properties_updated_cb(void (*cb)(wlc_handle view, uint32_t mask))`
- [x] `void wlc_set_keyboard_key_cb(bool (*cb)(wlc_handle view, uint32_t time, const struct wlc_modifiers*, uint32_t key, enum wlc_key_state))`
- [x] `void wlc_set_pointer_button_cb(bool (*cb)(wlc_handle view, uint32_t time, const struct wlc_modifiers*, uint32_t button, enum wlc_button_state, const struct wlc_point*))`
- [x] `void wlc_set_pointer_scroll_cb(bool (*cb)(wlc_handle view, uint32_t time, const struct wlc_modifiers*, uint8_t axis_bits, double amount[2]))`
- [x] `void wlc_set_pointer_motion_cb(bool (*cb)(wlc_handle view, uint32_t time, const struct wlc_point*))`
- [ ] `void wlc_set_touch_cb(bool (*cb)(wlc_handle view, uint32_t time, const struct wlc_modifiers*, enum wlc_touch_type, int32_t slot, const struct wlc_point*))`
- [ ] `void wlc_set_compositor_ready_cb(void (*cb)(void))`
- [ ] `void wlc_set_compositor_terminate_cb(void (*cb)(void))`
- [ ] `void wlc_set_input_created_cb(bool (*cb)(struct libinput_device *device))`
- [ ] `void wlc_set_input_destroyed_cb(void (*cb)(struct libinput_device *device))`

Core API
--------

- [x] `void wlc_log_set_handler(void (*cb)(enum wlc_log_type type, const char *str))`
- [x] `bool wlc_init(void)`
- [x] `void wlc_terminate(void)`
- [x] `enum wlc_backend_type wlc_get_backend_type(void)`
- [x] `void wlc_exec(const char *bin, char *const args[])`
- [x] `void wlc_run(void)`

### Not needed / internal

- `void wlc_handle_set_user_data(wlc_handle handle, const void *userdata)`
- `void* wlc_handle_get_user_data(wlc_handle handle)`
- `struct wlc_event_source* wlc_event_loop_add_fd(int fd, uint32_t mask, int (*cb)(int fd, uint32_t mask, void *userdata), void *userdata)`
- `struct wlc_event_source* wlc_event_loop_add_timer(int (*cb)(void *userdata), void *userdata)`
- `bool wlc_event_source_timer_update(struct wlc_event_source *source, int32_t ms_delay)`
- `void wlc_event_source_remove(struct wlc_event_source *source)`

Output API
----------

- [x] `const wlc_handle* wlc_get_outputs(size_t *out_memb)`
- [x] `wlc_handle wlc_get_focused_output(void)`
- [x] `const char* wlc_output_get_name(wlc_handle output)`
- [ ] `bool wlc_output_get_sleep(wlc_handle output)`
- [ ] `void wlc_output_set_sleep(wlc_handle output, bool sleep)`
- [x] `const struct wlc_size* wlc_output_get_resolution(wlc_handle output)`
- [x] `const struct wlc_size* wlc_output_get_virtual_resolution(wlc_handle output)`
- [x] `void wlc_output_set_resolution(wlc_handle output, const struct wlc_size *resolution, uint32_t scale)`
- [ ] `uint32_t wlc_output_get_scale(wlc_handle output)`
- [x] `uint32_t wlc_output_get_mask(wlc_handle output)`
- [x] `void wlc_output_set_mask(wlc_handle output, uint32_t mask)`
- [x] `const wlc_handle* wlc_output_get_views(wlc_handle output, size_t *out_memb)`
- [ ] `wlc_handle* wlc_output_get_mutable_views(wlc_handle output, size_t *out_memb)`
- [ ] `bool wlc_output_set_views(wlc_handle output, const wlc_handle *views, size_t memb)`
- [ ] `void wlc_output_focus(wlc_handle output)`

View API
--------

- [x] `void wlc_view_focus(wlc_handle view)`
- [x] `void wlc_view_close(wlc_handle view)`
- [x] `wlc_handle wlc_view_get_output(wlc_handle view)`
- [x] `void wlc_view_set_output(wlc_handle view, wlc_handle output)`
- [x] `void wlc_view_send_to_back(wlc_handle view)`
- [x] `void wlc_view_send_below(wlc_handle view, wlc_handle other)`
- [x] `void wlc_view_bring_above(wlc_handle view, wlc_handle other)`
- [x] `void wlc_view_bring_to_front(wlc_handle view)`
- [x] `uint32_t wlc_view_get_mask(wlc_handle view)`
- [x] `void wlc_view_set_mask(wlc_handle view, uint32_t mask)`
- [x] `const struct wlc_geometry* wlc_view_get_geometry(wlc_handle view)`
- [ ] `void wlc_view_get_visible_geometry(wlc_handle view, struct wlc_geometry *out_geometry)`
- [x] `void wlc_view_set_geometry(wlc_handle view, uint32_t edges, const struct wlc_geometry *geometry)`
- [ ] `uint32_t wlc_view_get_type(wlc_handle view)`
- [ ] `void wlc_view_set_type(wlc_handle view, enum wlc_view_type_bit type, bool toggle)`
- [x] `uint32_t wlc_view_get_state(wlc_handle view)`
- [x] `void wlc_view_set_state(wlc_handle view, enum wlc_view_state_bit state, bool toggle)`
- [ ] `wlc_handle wlc_view_get_parent(wlc_handle view)`
- [ ] `void wlc_view_set_parent(wlc_handle view, wlc_handle parent)`
- [x] `const char* wlc_view_get_title(wlc_handle view)`
- [ ] `const char* wlc_view_get_class(wlc_handle view)`
- [ ] `const char* wlc_view_get_app_id(wlc_handle view)`
- [ ] `pid_t wlc_view_get_pid(wlc_handle view)`

Input API
---------

- [ ] `struct xkb_state* wlc_keyboard_get_xkb_state(void)`
- [ ] `struct xkb_keymap* wlc_keyboard_get_xkb_keymap(void)`
- [x] `const uint32_t* wlc_keyboard_get_current_keys(size_t *out_memb)`
- [x] `uint32_t wlc_keyboard_get_keysym_for_key(uint32_t key, const struct wlc_modifiers *modifiers)`
- [ ] `uint32_t wlc_keyboard_get_utf32_for_key(uint32_t key, const struct wlc_modifiers *modifiers)`
- [ ] `void wlc_pointer_get_position(struct wlc_point *out_position)`
- [x] `void wlc_pointer_set_position(const struct wlc_point *position)`
