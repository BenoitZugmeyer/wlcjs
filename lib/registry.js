
const bindings = require("bindings")("wlc");
const DefaultMap = require("./DefaultMap");
const enums = require("./enums");
const mixEnumInto = require("./mixEnumInto");

class Handle {

  constructor(handle) {
    Object.defineProperty(this, "handle", { value: handle });
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

  mixEnumInto(enums.viewState, View, accessor(view.getState, view.setState));
  mixEnumInto(enums.viewType, View, accessor(view.getType, view.setType));

  const output = bindings.output;
  Object.defineProperties(Output.prototype, {
    name: accessor(output.getName),
    connectorType: accessor(output.getConnectorType),
    connectorId: accessor(output.getConnectorId),
    sleep: accessor(output.getSleep, output.setSleep),
    setResolution: method(output.setResolution),
    resolution: accessor(output.getResolution),
    virtualResolution: accessor(output.getVirtualResolution),
    mask: accessor(output.getMask, output.setMask),
    views: accessor(
      (handle) => output.getViews(handle).map((h) => views.get(h)),
      (handle, views) => output.setViews(handle, Array.from(views).map((v) => v.handle))
    ),
    focus: method(output.focus),
  });
  // TODO getPixels
}


const views = new DefaultMap((handle) => new View(handle));
views.set(0, undefined);  // prevent creating a view for the handle "0"
const outputs = new DefaultMap((handle) => new Output(handle));
outputs.set(0, undefined);  // prevent creating an output for the handle "0"

module.exports = {
  views,
  outputs,
};
