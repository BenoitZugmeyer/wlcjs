"use strict";

const bindings = require('bindings')('wlc');
const DefaultMap = require("./DefaultMap");

const modifierKeys = [ "Shift", "Caps", "Ctrl", "Alt", "Mod2", "Mod3", "Logo", "Mod5" ];
const modifierLeds = [ "Num", "Caps", "Scroll" ];

class KeyboardEvent {

  constructor(view, time, modifiers, keyId, state) {
    Object.defineProperties(this, {
      _modifiers: { value: modifiers },
      view: { value: view },
      time: { value: time },
      keyId: { value: keyId },
      state: { value: state },
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
      view: { value: view },
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

  get title() {
    return bindings.view.getTitle(this.handle);
  }

}

class Output extends Handle {

  get name() {
    return bindings.output.getName(this.handle);
  }

}

const wrapEvent =
  (Class, fn) =>
    fn && ((a, b, c, d, e, f, g, h) => fn(new Class(a, b, c, d, e, f, g, h)));

const views = new DefaultMap((handle) => new View(handle));
const outputs = new DefaultMap((handle) => new Output(handle));

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
exports.getBackendType = bindings.getBackendType;
exports.getOutputs = bindings.getOutputs;
exports.run = bindings.run;
exports.setLogHandler = bindings.setLogHandler;
exports.terminate = bindings.terminate;

exports.view = bindings.view;
exports.output = bindings.output;
