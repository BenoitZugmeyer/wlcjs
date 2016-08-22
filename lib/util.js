
function  mixEnumInto(enum_, Class, { set, get, suffix="", all }) {
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
}

module.exports = {
  mixEnumInto,
};
