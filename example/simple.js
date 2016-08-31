const wlc = require("..");

wlc.events.on("pointerMotion", (event) => {
  wlc.setPointerPosition(event.position);
});

wlc.init();
wlc.run();
