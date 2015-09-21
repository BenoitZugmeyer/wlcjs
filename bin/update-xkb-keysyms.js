#!/usr/bin/node
"use strict";

const fs = require("fs");
const path = require("path");
const request = require("https").request;

const headerURL = "https://raw.githubusercontent.com/xkbcommon/libxkbcommon/master/xkbcommon/xkbcommon-keysyms.h";

function* parseKeys(body) {
  const re = /^#define\s+XKB_KEY_(\w+)\s+0x(\w+)/gm;
  const yielded = new Set();
  while (true) {
    const match = re.exec(body);
    if (!match) break;
    const key = parseInt(match[2], 16);
    if (!yielded.has(key)) {
      yield match[1];
      yielded.add(key);
    }
  }
}

function writeKeys(keys) {
  const file = path.resolve(__dirname, "../src/keysym_to_string.cc");
  const fd = fs.openSync(file, "w");
  try {
    fs.writeSync(
      fd,
      `#include "wlcjs.h"\n\nnamespace wlcjs {\nconst char* keysym_to_string(uint32_t keysym) {\n  switch (keysym) {\n`
    );
    for (const name of keys) fs.writeSync(fd, `    case XKB_KEY_${name}: return "${name}";\n`);
    fs.writeSync( fd, `    default: return "unknown";\n  }\n}\n}\n`);
  }
  catch (e) {
    console.log(`Error while writing: ${e.stack}`);
  }
  finally {
    fs.closeSync(fd);
  }
}

request(headerURL, (response) => {
  if (response.statusCode !== 200) {
    process.stderr.write(`HTTP status ${response.statusCode} while fetching header ${headerURL}\n`);
    process.exit(1);
  }

  const buffers = [];
  response.on("data", (data) => buffers.push(data));
  response.on("end", () => writeKeys(parseKeys(Buffer.concat(buffers).toString("utf-8"))));
}).end();
