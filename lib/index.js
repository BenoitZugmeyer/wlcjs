"use strict";

const bindings = require("bindings")("wlc");
const DefaultMap = require("./DefaultMap");
const enums = require("./enums");

const mixBitEnum = (Class, enum_, options) => {
  const set = options.set;
  const get = options.get;
  const suffix = options.suffix || "";
  const all = options.all;

  for (const name in enum_) {
    if (name === "none") continue;
    const bit = enum_[name];
    Object.defineProperty(Class.prototype, name + suffix, {
      get: get && function () { return Boolean(get.call(this) & bit); },
      set: set && function (value) { set.call(this, bit, value); },
    });
  }

  if (all) {
    Object.defineProperty(Class.prototype, all, {
      get() {
        return new Set(enum_.formatAsArray(get.call(this)));
      },
    });
  }
};

class Event {

  constructor(view, time, modifiers) {
    Object.defineProperties(this, {
      view: { value: views.get(view) },
      time: { value: time },
      _modifiers: { value: modifiers },
    });
  }

}

mixBitEnum(Event, enums.modifier, {
  get() { return this._modifiers.mods; },
  suffix: "Key",
  all: "modifierKeys",
});

mixBitEnum(Event, enums.led, {
  get() { return this._modifiers.leds; },
  suffix: "Led",
  all: "modifierLeds",
});

class KeyboardEvent extends Event {

  constructor(view, time, modifiers, keyId, state) {
    super(view, time, modifiers);
    Object.defineProperties(this, {
      keyId: { value: keyId },
      state: { value: enums.keyState.format(state) },
    });
  }

  get key() {
    return bindings.input.getKeysymNameForKey(this.keyId);
  }

  get fullKey() {
    const mk = this.modifierKeys;
    return mk.size ? `${Array.from(mk).join("-")}-${this.key}` : this.key;
  }

  toString() {
    return `KeyboardEvent { key: ${this.fullKey} }`;
  }

}

class PointerMotionEvent extends Event {

  constructor(view, time, position) {
    super(view, time);
    Object.defineProperties(this, {
      position: { value: position },
    });
  }

}

class PointerButtonEvent extends Event {

  constructor(view, time, modifiers, button, state, position) {
    super(view, time, modifiers);

    Object.defineProperties(this, {
      button: { value: enums.button.format(button) },
      state: { value: enums.buttonState.format(state) },
      position: { value: position },
    });
  }

}


class PointerScrollEvent extends Event {

  constructor(view, time, modifiers, axis, amount) {
    super(view, time, modifiers);
    Object.defineProperties(this, {
      _axis: { value: axis },
      _amount: { value: amount },
    });
  }

  get vertical() {
    return this._axis & enums.scrollAxis.vertical ? this._amount[0] : 0;
  }

  get horizontal() {
    return this._axis & enums.scrollAxis.horizontal ? this._amount[1] : 0;
  }

}

class Handle {

  constructor(handle) {
    Object.defineProperty(this, "handle", { value: handle });
  }

}

class View extends Handle {
}

class Output extends Handle {
}

{
  const handleBinding = (binding) =>
    function (a, b, c, d) {
      return binding.call(null, this.handle, a, b, c, d);
    };

  const accessor = (getter, setter) => ({
    get: handleBinding(getter),
    set: arguments.length >= 2 && handleBinding(setter),
  });

  const method = (fn) => ({
    value: handleBinding(fn),
  });

  const view = bindings.view;
  Object.defineProperties(View.prototype, {
    title: accessor(view.getTitle, view.setTitle),
    class: accessor(view.getClass, view.setClass),
    appId: accessor(view.getAppId, view.setAppId),
    focus: method(view.focus),
    close: method(view.close),
    output: accessor(
      (handle) => outputs.get(view.getOutput(handle)),
      (handle, other) => view.setOutput(handle, other.handle)
    ),
    sendToBack: method(view.sendToBack),
    sendBelow: method((handle, other) => view.sendToBack(handle, other.handle)),
    bringAbove: method((handle, other) => view.bringAbove(handle, other.handle)),
    bringToFront: method(view.bringToFront),
    mask: accessor(view.getMask, view.setMask),
    setGeometry: method(view.setGeometry),
    geometry: accessor(view.getGeometry),
    parent: accessor(
      (handle) => views.get(view.getParent(handle)),
      (handle, other) => view.setParent(handle, other.handle)
    ),
  });

  mixBitEnum(View, enums.viewState, accessor(view.getState, view.setState));
  mixBitEnum(View, enums.viewType, accessor(view.getType, view.setType));

  const output = bindings.output;
  Object.defineProperties(Output.prototype, {
    name: accessor(output.getName),
    connectorType: accessor(output.getConnectorType),
    connectorId: accessor(output.getConnectorId),
    sleep: accessor(output.getSleep, output.setSleep),
    resolution: accessor(output.getResolution, output.setResolution),
    mask: accessor(output.getMask, output.setMask),
    views: accessor(
      (handle) => output.getViews(handle).map((h) => views.get(h)),
      (handle, views) => output.setViews(handle, Array.from(views).map((v) => v.handle))
    ),
    focus: method(output.focus),
  });
  // TODO getPixels
}

const wrapEvent =
  (Class) =>
    (fn, ...args) =>
      fn && fn(new Class(...args));

const views = new DefaultMap((handle) => new View(handle));
views.set(0, undefined);
const outputs = new DefaultMap((handle) => new Output(handle));
outputs.set(0, undefined);

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
