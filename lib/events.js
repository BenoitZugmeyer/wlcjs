
const { EventEmitter } = require("events");
const bindings = require("bindings")("wlc");
const enums = require("./enums");
const mixEnumInto = require("./mixEnumInto");
const { views, outputs } = require("./registry");

class Event {
  constructor(defaultReturnValue) {
    Object.defineProperties(this, {
      _returnValue: { value: defaultReturnValue, writable: true },
      _defaultReturnValue: { value: defaultReturnValue },
    });
  }

  preventDefault() {
    if (this._defaultReturnValue === undefined) {
      throw new Error("Default could not be prevented");
    }

    this._returnValue = !this._defaultReturnValue;
  }

  isDefaultPrevented() {
    return this._returnValue !== this._defaultReturnValue;
  }
}

class InputEvent extends Event {
  constructor(viewHandle, time, modifiers) {
    super(false);
    Object.defineProperties(this, {
      view: { value: views.get(viewHandle) },
      time: { value: time },
      _modifiers: { value: modifiers },
    });
  }
}

mixEnumInto(enums.modifier, InputEvent, {
  get() { return this._modifiers.mods; },
  suffix: "Key",
  all: "modifierKeys",
});

mixEnumInto(enums.led, InputEvent, {
  get() { return this._modifiers.leds; },
  suffix: "Led",
  all: "modifierLeds",
});

class KeyboardKeyEvent extends InputEvent {
  constructor(viewHandle, time, modifiers, keyId, state) {
    super(viewHandle, time, modifiers);
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
}

class PointerMotionEvent extends InputEvent {
  constructor(viewHandle, time, position) {
    super(viewHandle, time);
    Object.defineProperties(this, {
      position: { value: position },
    });
  }
}

class PointerButtonEvent extends InputEvent {
  constructor(viewHandle, time, modifiers, button, state, position) {
    super(viewHandle, time, modifiers);

    Object.defineProperties(this, {
      button: { value: enums.button.format(button) },
      state: { value: enums.buttonState.format(state) },
      position: { value: position },
    });
  }
}


class PointerScrollEvent extends InputEvent {
  constructor(viewHandle, time, modifiers, axis, amount) {
    super(viewHandle, time, modifiers);
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

class OutputEvent extends Event {
  constructor(outputHandle, defaultReturnValue) {
    super(defaultReturnValue);
    Object.defineProperty(this, "output", { value: outputs.get(outputHandle) });
  }
}

class OutputCreatedEvent extends OutputEvent {
  constructor(outputHandle) {
    super(outputHandle, true);
  }
}

class OutputDestroyedEvent extends OutputEvent {
  constructor(outputHandle) {
    super(outputHandle);
  }
}

class OutputFocusEvent extends OutputEvent {
  constructor(outputHandle, focus) {
    super(outputHandle);
    Object.defineProperty(this, "focus", { value: focus });
  }
}

class OutputResolutionEvent extends OutputEvent {
  constructor(outputHandle, from, to) {
    super(outputHandle);
    Object.defineProperties(this, {
      from: { value: from },
      to: { value: to },
    });
  }
}

class ViewEvent extends Event {
  constructor(viewHandle, defaultReturnValue) {
    super(defaultReturnValue);
    Object.defineProperty(this, "view", { value: views.get(viewHandle) });
  }
}

class ViewCreatedEvent extends ViewEvent {
  constructor(viewHandle) {
    super(viewHandle, true);
  }
}

class ViewDestroyedEvent extends ViewEvent {
  constructor(viewHandle) {
    super(viewHandle);
  }
}

class ViewFocusEvent extends ViewEvent {
  constructor(viewHandle, focus) {
    super(viewHandle);
    Object.defineProperty(this, "focus", { value: focus });
  }
}

class ViewMoveToOutputEvent extends ViewEvent {
  constructor(viewHandle, fromOutputHandle, toOutputHandle) {
    super(viewHandle);
    Object.defineProperties(this, {
      from: { value: outputs.get(fromOutputHandle) },
      to: { value: outputs.get(toOutputHandle) },
    });
  }
}

class ViewRequestMoveEvent extends ViewEvent {
  constructor(viewHandle, point) {
    super(viewHandle);
    Object.defineProperties(this, {
      point: { value: point },
    });
  }
}


const events = new EventEmitter();

const eventTypes = {
  outputCreated: OutputCreatedEvent,
  outputDestroyed: OutputDestroyedEvent,
  outputFocus: OutputFocusEvent,
  outputResolution: OutputResolutionEvent,
  keyboardKey: KeyboardKeyEvent,
  pointerButton: PointerButtonEvent,
  pointerScroll: PointerScrollEvent,
  pointerMotion: PointerMotionEvent,
  viewCreated: ViewCreatedEvent,
  viewDestroyed: ViewDestroyedEvent,
  viewFocus: ViewFocusEvent,
  viewMoveToOutput: ViewMoveToOutputEvent,
  viewRequestMove: ViewRequestMoveEvent,
};

events.on("removeListener", (eventName) => {
  if (events.listenerCount(eventName) === 0) {
    delete bindings.callbacks[eventName];
  }
});

events.on("newListener", (eventName) => {
  if (!eventTypes.hasOwnProperty(eventName)) {
    throw new Error(`Unknown event type "${eventName}"`);
  }
  if (bindings.callbacks[eventName]) return;
  bindings.callbacks[eventName] = (...args) => {
    const event = new eventTypes[eventName](...args);
    events.emit(eventName, event);
    return event._returnValue;
  };
});

module.exports = events;
