/**
 * This example is a direct port of the WLC example/example.c, to demonstrate
 * the differences between the C API and the JS bindings.  It will follow
 * evolutions of the JS bindings and the upstream example.
 **/

const wlc = require("..");

const compositor = {
  action: {
    view: null,
    grab: null,
    edges: 0,
  },
};

function startInteractiveAction(view, origin) {
  if (compositor.action.view) return false;

  compositor.action.view = view;
  compositor.action.grab = origin;
  view.bringToFront();
  return true;
}

function startInteractiveMove(view, origin) {
  startInteractiveAction(view, origin);
}

function startInteractiveResize(view, edges, origin) {
  const g = view.geometry;
  if (!g || !startInteractiveAction(view, origin)) return;

  const halfw = g.x + g.width / 2;
  const halfh = g.y + g.height / 2;

  compositor.action.edges = edges;

  if (!edges) {
    compositor.action.edges =
      (origin.x < halfw ? wlc.enums.resizeEdge.left : (origin.x > halfw ?  wlc.enums.resizeEdge.right : 0)) |
      (origin.y < halfh ? wlc.enums.resizeEdge.top : (origin.y > halfh ? wlc.enums.resizeEdge.bottom : 0));
  }

  view.resizing = true;
}

function stopInteractiveAction() {
  if (!compositor.action.view) return;

  compositor.action.view.resizing = false;
  compositor.action = { view: null, grab: null, edges: 0 };
}

function getTopmost(output, offset) {
  const views = output.views;
  return (views.length > 0 ? views[(views.length - 1 + offset) % views.length] : 0);
}

function relayout(output) {
  // very simple layout function
  // you probably don't want to layout certain type of windows in wm

  const r = output.virtualResolution;
  if (!r) return;

  const views = output.views;

  let toggle = false;
  let y = 0;
  const n = Math.floor((1 + views.length) / 2);
  const width = r.width / 2, height = r.height / n;
  const ew = r.width - width * 2, eh = r.height - height * n;
  for (let i = 0; i < views.length; ++i) {
    const g = {
      x: (toggle ? width + ew : 0),
      y,
      width: (!toggle && i === views.length - 1 ? r.width : (toggle ? width : width + ew)),
      height: (i < 2 ? height + eh : height),
    };
    views[i].setGeometry(0, g);
    toggle = !toggle;
    y = y + (!toggle ? g.height : 0);
  }
}

function outputResolution(event) {
  relayout(event.output);
}

function viewCreated(event) {
  const view = event.view;
  view.mask = view.output.mask;
  view.bringToFront();
  view.focus();
  relayout(view.output);
}

function viewDestroyed(event) {
  const output = event.view.output;
  if (!output) return;
  const topmost = getTopmost(output, 0);
  if (topmost) topmost.focus();
  relayout(output);
}

function viewFocus(event) {
  event.view.activated = event.focus;
}

function viewRequestMove(event) {
  startInteractiveMove(event.view, event.point);
}

function viewRequestResize(event) {
  startInteractiveResize(event.view, event.edges, event.point);
}

function viewRequestGeometry() {
   // stub intentionally to ignore geometry requests.
}

function keyboardKey(event) {
  const sym = event.key;

  if (event.state === "pressed") {
    if (event.view) {
      if (event.ctrlKey && sym === "q") {
        event.view.close();
        event.preventDefault();
        return;
      } else if (event.ctrlKey && sym === "Down") {
        event.view.sendToBack();
        getTopmost(event.view.output, 0).focus();
        event.preventDefault();
        return;
      }
    }

    if (event.ctrlKey && sym === "Escape") {
      wlc.terminate();
      event.preventDefault();
      return;
    } else if (event.ctrlKey && sym == "Return") {
      const terminal = process.env["TERMINAL"] || "weston-terminal";
      wlc.exec(terminal);
      event.preventDefault();
      return;
    } else if (event.ctrlKey && sym >= 1 && sym <= 9) {
      const outputs = wlc.getOutputs();
      const scale = parseInt(sym, 10);

      for (const output of outputs) {
        output.setResolution(output.resolution, scale);
      }

      event.preventDefault();
      return;
    }
  }
}

function pointerButton(event) {
  if (event.state === "pressed") {
    if (event.view) {
      event.view.focus();
      if (event.ctrlKey && event.button === "left") {
        startInteractiveMove(event.view, event.position);
      }
      if (event.ctrlKey && event.button === "right") {
        startInteractiveResize(event.view, 0, event.position);
      }
    }
  } else {
    stopInteractiveAction();
  }

  if (compositor.action.view) event.preventDefault();
}

function pointerMotion(event) {
  const position = event.position;

  if (compositor.action.view) {
    const dx = position.x - compositor.action.grab.x;
    const dy = position.y - compositor.action.grab.y;
    const g = compositor.action.view.geometry;

    if (compositor.action.edges) {
      const min = { width: 80, height: 40 };

      const n = g;
      if (compositor.action.edges & wlc.enums.resizeEdge.left) {
        n.width -= dx;
        n.x += dx;
      } else if (compositor.action.edges & wlc.enums.resizeEdge.right) {
        n.width += dx;
      }

      if (compositor.action.edges & wlc.enums.resizeEdge.top) {
        n.height -= dy;
        n.y += dy;
      } else if (compositor.action.edges & wlc.enums.resizeEdge.bottom) {
        n.height += dy;
      }

      if (n.width >= min.width) {
        g.x = n.x;
        g.width = n.width;
      }

      if (n.height >= min.height) {
        g.y = n.y;
        g.height = n.height;
      }

      compositor.action.view.setGeometry(compositor.action.edges, g);
    } else {
      g.x += dx;
      g.y += dy;
      compositor.action.view.setGeometry(0, g);
    }

    compositor.action.grab = position;
  }

  // In order to give the compositor control of the pointer placement it needs
  // to be explicitly set after receiving the motion event:
  wlc.setPointerPosition(position);
  if (compositor.action.view) event.preventDefault();
}

function cbLog(type, str) {
  console.log(`${type}: ${str}`);  // eslint-disable-line no-console
}

function main() {
  wlc.setLogHandler(cbLog);

  wlc.events.on("outputResolution", outputResolution);
  wlc.events.on("viewCreated", viewCreated);
  wlc.events.on("viewDestroyed", viewDestroyed);
  wlc.events.on("viewFocus", viewFocus);
  wlc.events.on("viewRequestMove", viewRequestMove);
  wlc.events.on("viewRequestResize", viewRequestResize);
  wlc.events.on("viewRequestGeometry", viewRequestGeometry);
  wlc.events.on("keyboardKey", keyboardKey);
  wlc.events.on("pointerButton", pointerButton);
  wlc.events.on("pointerMotion", pointerMotion);

  wlc.init();
  wlc.run();
}

main();
