"use strict";

class Enum {

  constructor(names, options) {
    Object.defineProperty(this, "_unfinished", { value: options && options.unfinished === true });
    const define = (name, value) => Object.defineProperty(this, name, { value, enumerable: true });
    if (Array.isArray(names)) names.forEach(define);
    else Object.keys(names).forEach((name) => define(name, names[name]));
  }

  format(value) {
    for (const v in this) {
      if (this[v] === value) return v;
    }
    if (this._unfinished) {
      return `0x${value.toString(16)}`;
    }
    throw new Error(`${value} not found in enum`);
  }

}

class BitEnum extends Enum {

  constructor(names) {
    const n = { none: 0 };
    names.forEach((name, i) => n[name] = 1 << i);
    super(n);
  }

  formatAsArray(value) {
    const result = [];
    for (const v in this) {
      if (value & this[v]) result.push(v);
    }
    return result;
  }

  format(value) {
    const result = this.formatAsArray(value);
    return result.length ? result.join("|") : "none";
  }

}

exports.log = new Enum([
  "info",
  "warn",
  "error",
  "wayland",
]);

exports.backend = new Enum([
  "none",
  "drm",
  "x11",
]);

exports.event = new BitEnum([
  "readable",
  "writable",
  "hangup",
  "error",
]);

exports.viewState = new BitEnum([
  "maximized",
  "fullscreen",
  "resizing",
  "moving",
  "activated",
]);

exports.viewType = new BitEnum([
  "overrideRedirect",
  "unmanaged",
  "splash",
  "modal",
  "popup",
]);

exports.viewPropertyUpdate = new BitEnum([
  "title",
  "class",
  "appId",
  "pid",
]);

exports.resizeEdge = new BitEnum([
  "top",
  "bottom",
  "left",
  "right",
]);

exports.modifier = new BitEnum([
  "shift",
  "caps",
  "ctrl",
  "alt",
  "mod2",
  "mod3",
  "logo",
  "mod5",
]);

exports.led = new BitEnum([
  "num",
  "caps",
  "scroll",
]);

exports.keyState = new Enum([
  "released",
  "pressed",
]);

exports.buttonState = new Enum([
  "released",
  "pressed",
]);

exports.scrollAxis = new BitEnum([
  "vertical",
  "horizontal",
]);

exports.touchType = new Enum([
  "down",
  "up",
  "motion",
  "frame",
  "cancel",
]);

// See linux/linux.h
exports.button = new Enum({
  "left": 0x110,
  "right": 0x111,
  "middle": 0x112,
  "side": 0x113,
  "extra": 0x114,
  "forward": 0x115,
  "back": 0x116,
  "task": 0x117,
}, { unfinished: true });
