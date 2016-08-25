"use strict";

const bindings = require("bindings")("wlc");
const { outputs } = require("./registry");
const enums = require("./enums");

module.exports = {
  events: require("./events"),
  init: bindings.init,
  exec: bindings.exec,
  getBackendType() {
    return enums.backend.format(bindings.getBackendType());
  },
  getOutputs() {
    return bindings.getOutputs().map((handle) => outputs.get(handle));
  },
  getFocusedOutput() {
    return outputs.get(bindings.getFocusedOutput());
  },
  getCurrentKeys: bindings.input.getCurrentKeys,
  run: bindings.run,
  setLogHandler(fn) {
    bindings.setLogHandler((type, s) => fn(enums.log.format(type), s));
  },
  terminate: bindings.terminate,
};
