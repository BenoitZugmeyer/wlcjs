"use strict";

const bindings = require('bindings')('wlc');
const DefaultMap = require("./DefaultMap");
const enums = require("./enums");

const modifierKeys = [ "Shift", "Caps", "Ctrl", "Alt", "Mod2", "Mod3", "Logo", "Mod5" ];
const modifierLeds = [ "Num", "Caps", "Scroll" ];

class KeyboardEvent {

  constructor(view, time, modifiers, keyId, state) {
    Object.defineProperties(this, {
      _modifiers: { value: modifiers },
      view: { value: views.get(view) },
      time: { value: time },
      keyId: { value: keyId },
      state: { value: enums.keyState.format(state) },
    });
  }

  get key() {
    return bindings.getKeysymNameForKey(this.keyId);
  }

  get fullKey() {
    const mk = this.modifierKeys;
    return mk.length ? `${mk.join("-")}-${this.key}` : this.key;
  }

  get modifierKeys() {
    return modifierKeys.filter((name, i) => this._modifiers.mods & (1 << i));
  }

  get modifierLeds() {
    return modifierLeds.filter((name, i) => this._modifiers.leds & (1 << i));
  }

  toString() {
    return `KeyboardEvent { key: ${this.key} }`;
  }

}

modifierKeys.forEach((name, i) => {
  const value = 1 << i;
  Object.defineProperty(KeyboardEvent.prototype, name.toLowerCase() + "Key", {
    get: function () { return this._modifiers.mods & value; },
  });
});

modifierLeds.forEach((name, i) => {
  const value = 1 << i;
  Object.defineProperty(KeyboardEvent.prototype, name.toLowerCase() + "Led", {
    get: function () { return this._modifiers.leds & value; },
  });
});

class PointerMotionEvent {

  constructor(view, time, position) {
    Object.defineProperties(this, {
      view: { value: views.get(view) },
      time: { value: time },
      position: { value: position },
    });
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

  const mixBitField = (definitions, enum_, getter, setter) => {
    for (const name in enum_) {
      const bit = enum_[name];
      viewDefinitions[name] = accessor(
        (handle) => Boolean(getter(handle) & bit),
        (handle, value) => setter(handle, bit, value)
      );
    }

  };

  const view = bindings.view;
  const viewDefinitions = {
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
  }

  mixBitField(viewDefinitions, enums.viewState, view.getState, view.setState);
  mixBitField(viewDefinitions, enums.viewType, view.getType, view.setType);

  Object.defineProperties(View.prototype, viewDefinitions);

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
    fn && ((handle) => fn(map.get(handle)));

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
