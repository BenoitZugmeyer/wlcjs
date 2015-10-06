TODO
====

- [x] `wlc_init(const struct wlc_interface *interface, int argc, char *argv[]) -> bool`
- [x] `wlc_run(void) -> void`
- [x] `wlc_log_set_handler(void (*cb)(enum wlc_log_type type, const char *str)) -> void`
- [x] `wlc_terminate(void) -> void`
- [x] `wlc_get_backend_type(void) -> enum wlc_backend_type`
- [x] `wlc_exec(const char *bin, char *const args[]) -> void`
- [ ] `wlc_keyboard_get_xkb_state(void) -> struct xkb_state*`
- [ ] `wlc_keyboard_get_xkb_keymap(void) -> struct xkb_keymap*`
- [x] `wlc_keyboard_get_current_keys(size_t *out_memb) -> const uint32_t*`
- [x] `wlc_keyboard_get_keysym_for_key(uint32_t key, const struct wlc_modifiers *modifiers) -> uint32_t`
- [ ] `wlc_keyboard_get_utf32_for_key(uint32_t key, const struct wlc_modifiers *modifiers) -> uint32_t`
- [ ] `wlc_pointer_get_origin(struct wlc_origin *out_origin) -> void`

Output
------

- [x] `wlc_get_outputs(size_t *out_memb) -> const wlc_handle*`
- [x] `wlc_get_focused_output(void) -> wlc_handle`
- [x] `wlc_output_get_name(wlc_handle output) -> const char*`
- [ ] `wlc_output_get_connector_type(wlc_handle output) -> enum wlc_connector_type`
- [ ] `wlc_output_get_connector_id(wlc_handle output) -> uint32_t`
- [ ] `wlc_output_get_sleep(wlc_handle output) -> bool`
- [ ] `wlc_output_set_sleep(wlc_handle output, bool sleep) -> void`
- [x] `wlc_output_get_resolution(wlc_handle output) -> const struct wlc_size*`
- [ ] `wlc_output_set_resolution(wlc_handle output, const struct wlc_size *resolution) -> void`
- [ ] `wlc_output_get_mask(wlc_handle output) -> uint32_t`
- [ ] `wlc_output_set_mask(wlc_handle output, uint32_t mask) -> void`
- [ ] `wlc_output_get_pixels(wlc_handle output, bool (*pixels)(const struct wlc_size *size, uint8_t *rgba, void *arg), void *arg) -> void`
- [x] `wlc_output_get_views(wlc_handle output, size_t *out_memb) -> const wlc_handle*`
- [ ] `wlc_output_get_mutable_views(wlc_handle output, size_t *out_memb) -> wlc_handle*`
- [ ] `wlc_output_set_views(wlc_handle output, const wlc_handle *views, size_t memb) -> bool`
- [ ] `wlc_output_focus(wlc_handle output) -> void`

View
----

- [x] `wlc_view_focus(wlc_handle view) -> void`
- [x] `wlc_view_close(wlc_handle view) -> void`
- [x] `wlc_view_get_output(wlc_handle view) -> wlc_handle`
- [x] `wlc_view_set_output(wlc_handle view, wlc_handle output) -> void`
- [x] `wlc_view_send_to_back(wlc_handle view) -> void`
- [x] `wlc_view_send_below(wlc_handle view, wlc_handle other) -> void`
- [ ] `wlc_view_bring_above(wlc_handle view, wlc_handle other) -> void`
- [x] `wlc_view_bring_to_front(wlc_handle view) -> void`
- [ ] `wlc_view_get_mask(wlc_handle view) -> uint32_t`
- [ ] `wlc_view_set_mask(wlc_handle view, uint32_t mask) -> void`
- [ ] `wlc_view_get_geometry(wlc_handle view) -> const struct wlc_geometry*`
- [x] `wlc_view_set_geometry(wlc_handle view, uint32_t edges, const struct wlc_geometry *geometry) -> void`
- [ ] `wlc_view_get_type(wlc_handle view) -> uint32_t`
- [ ] `wlc_view_set_type(wlc_handle view, enum wlc_view_type_bit type, bool toggle) -> void`
- [x] `wlc_view_get_state(wlc_handle view) -> uint32_t`
- [x] `wlc_view_set_state(wlc_handle view, enum wlc_view_state_bit state, bool toggle) -> void`
- [ ] `wlc_view_get_parent(wlc_handle view) -> wlc_handle`
- [ ] `wlc_view_set_parent(wlc_handle view, wlc_handle parent) -> void`
- [x] `wlc_view_get_title(wlc_handle view) -> const char*`
- [ ] `wlc_view_set_title(wlc_handle view, const char *title) -> bool`
- [ ] `wlc_view_get_class(wlc_handle view) -> const char*`
- [ ] `wlc_view_set_class(wlc_handle view, const char *class_) -> bool`
- [ ] `wlc_view_get_app_id(wlc_handle view) -> const char*`
- [ ] `wlc_view_set_app_id(wlc_handle view, const char *app_id) -> bool`

Not necessary useful / internal
-------------------------------

- [ ] `wlc_handle_set_user_data(wlc_handle handle, const void *userdata) -> void`
- [ ] `wlc_handle_get_user_data(wlc_handle handle) -> void*`
- [ ] `wlc_event_loop_add_fd(int fd, uint32_t mask, int (*cb)(int fd, uint32_t mask, void *arg), void *arg) -> struct wlc_event_source*`
- [ ] `wlc_event_loop_add_timer(int (*cb)(void *arg), void *arg) -> struct wlc_event_source*`
- [ ] `wlc_event_source_timer_update(struct wlc_event_source *source, int32_t ms_delay) -> bool`
- [ ] `wlc_event_source_remove(struct wlc_event_source *source) -> void`
