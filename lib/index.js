"use strict";

const bindings = require('bindings')('wlc');

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

const wrapEvent =
  (Class, fn) =>
    fn && ((a, b, c, d, e, f, g, h) => fn(new Class(a, b, c, d, e, f, g, h)));

exports.init = (iface) => {
  if (!iface || typeof iface !== "object") throw new TypeError("Argument 1 must be an object");

  iface = Object.assign({}, iface);

  iface.keyboardKey = wrapEvent(KeyboardEvent, iface.keyboardKey);
  iface.pointerMotion = wrapEvent(PointerMotionEvent, iface.pointerMotion);

  bindings.init(iface);
};

exports.exec = bindings.exec;
exports.getBackendType = bindings.getBackendType;
exports.getOutputs = bindings.getOutputs;
exports.run = bindings.run;
exports.setLogHandler = bindings.setLogHandler;
exports.terminate = bindings.terminate;
