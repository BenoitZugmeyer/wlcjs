"use strict";

class DefaultMap extends Map {

  constructor(factory) {
    super();
    Object.defineProperty(this, "factory", { value: factory });
  }

  get(key) {
    if (!this.has(key)) this.set(key, this.factory(key));
    return super.get(key);
  }

}

module.exports = DefaultMap;
