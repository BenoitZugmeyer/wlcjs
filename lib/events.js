
const bindings = require("bindings")("wlc");
const enums = require("./enums");
const { mixEnumInto } = require("./util");
const { views } = require("./registry");

class Event {

  constructor(view, time, modifiers) {
    Object.defineProperties(this, {
      view: { value: views.get(view) },
      time: { value: time },
      _modifiers: { value: modifiers },
    });
  }

}

mixEnumInto(enums.modifier, Event, {
  get() { return this._modifiers.mods; },
  suffix: "Key",
  all: "modifierKeys",
});

mixEnumInto(enums.led, Event, {
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


module.exports = {
  PointerMotionEvent,
  PointerButtonEvent,
  PointerScrollEvent,
  KeyboardEvent,
};
