"use strict";

const bindings = require('bindings')('wlc');
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
      get: get && function () { return Boolean(get.call(this) & bit) },
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
    return bindings.getKeysymNameForKey(this.keyId);
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
    Object.defineProperty(this, 'handle', { value: handle });
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
  (Class, fn) =>
    fn && ((a, b, c, d, e, f, g, h) => fn(new Class(a, b, c, d, e, f, g, h)));

const views = new DefaultMap((handle) => new View(handle));
views.set(0, undefined);
const outputs = new DefaultMap((handle) => new Output(handle));
outputs.set(0, undefined);

const wrapHandle =
  (map, fn) =>
    fn && ((handle, a, b, c, d, e) => fn(map.get(handle), a, b, c, d, e));

const wrapDestroyHandle =
  (map, fn) =>
    (handle) => {
      if (fn) fn(map.get(handle));
      map.delete(handle);
    };

exports.init = (iface) => {
  if (!iface || typeof iface !== "object") throw new TypeError("Argument 1 must be an object");

  iface = Object.assign({}, iface);

  iface.keyboardKey = wrapEvent(KeyboardEvent, iface.keyboardKey);
  iface.pointerMotion = wrapEvent(PointerMotionEvent, iface.pointerMotion);
  iface.pointerButton = wrapEvent(PointerButtonEvent, iface.pointerButton);
  iface.pointerScroll = wrapEvent(PointerScrollEvent, iface.pointerScroll);

  iface.outputCreated = wrapHandle(outputs, iface.outputCreated);
  iface.outputFocus = wrapHandle(outputs, iface.outputFocus);
  iface.outputResolution = wrapHandle(outputs, iface.outputResolution);
  iface.outputDestroyed = wrapDestroyHandle(outputs, iface.outputDestroyed);

  iface.viewCreated = wrapHandle(views, iface.viewCreated);
  iface.viewFocus = wrapHandle(views, iface.viewFocus);
  iface.viewMoveToOutput = wrapHandle(views, iface.viewMoveToOutput);
  iface.viewDestroyed = wrapDestroyHandle(views, iface.viewDestroyed);

  bindings.init(iface);
};

exports.exec = bindings.exec;
exports.getBackendType = () => enums.backend.format(bindings.getBackendType());
exports.getOutputs = bindings.getOutputs;
exports.run = bindings.run;
exports.setLogHandler = (fn) => bindings.setLogHandler((type, s) => fn(enums.log.format(type), s));
exports.terminate = bindings.terminate;
