"use strict";

const bindings = require("bindings")("wlc");
const {outputs, views} = require("./registry");
const enums = require("./enums");
const {
  KeyboardEvent,
  PointerButtonEvent,
  PointerScrollEvent,
  PointerMotionEvent,
} = require("./events");

const wrapEvent =
  (Class) =>
    (fn, ...args) =>
      fn && fn(new Class(...args));


const wrapHandle =
  (map) =>
    (fn, handle, ...extra) =>
      fn && fn(map.get(handle), ...extra);

const wrapDestroyHandle =
  (map) =>
    (fn, handle) => {
      if (fn) fn(map.get(handle));
      map.delete(handle);
    };

exports.callbacks = {};

function defineCallback(name, handler) {
  bindings.callbacks[name] = (...args) => handler(exports.callbacks[name], ...args);
}

defineCallback("outputCreated", wrapHandle(outputs));
defineCallback("outputDestroyed", wrapDestroyHandle(outputs));
defineCallback("outputFocus", wrapHandle(outputs));
defineCallback("outputResolution", wrapHandle(outputs));

defineCallback("viewCreated", wrapHandle(views));
defineCallback("viewDestroyed", wrapDestroyHandle(views));
defineCallback("viewFocus", wrapHandle(views));
defineCallback("viewMoveToOutput", (fn, view, from, to) => fn && fn(views.get(view), outputs.get(from), outputs.get(to)));

defineCallback("keyboardKey", wrapEvent(KeyboardEvent));
defineCallback("pointerButton", wrapEvent(PointerButtonEvent));
defineCallback("pointerScroll", wrapEvent(PointerScrollEvent));
defineCallback("pointerMotion", wrapEvent(PointerMotionEvent));

exports.init = bindings.init;
exports.exec = bindings.exec;
exports.getBackendType = () => enums.backend.format(bindings.getBackendType());
exports.getOutputs = () => bindings.getOutputs().map((handle) => outputs.get(handle));
exports.getFocusedOutput = () => outputs.get(bindings.getFocusedOutput());
exports.getCurrentKeys = bindings.input.getCurrentKeys;
exports.run = bindings.run;
exports.setLogHandler = (fn) => bindings.setLogHandler((type, s) => fn(enums.log.format(type), s));
exports.terminate = bindings.terminate;
