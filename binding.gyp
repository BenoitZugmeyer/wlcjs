{
  "targets": [
    {
      "target_name": "wlc",
      "sources": [
        "src/wlcjs.cc",
        "src/callbacks.cc",
        "src/output.cc",
        "src/view.cc",
        "src/core.cc",
        "src/input.cc",
        "src/state.cc",
      ],
      "defines": [
        # "V8_DEPRECATION_WARNINGS=1",
        # "V8_IMMINENT_DEPRECATION_WARNINGS=1",
      ],
      "libraries": [
        "-lwlc"
      ]
    }
  ]
}

# vim: set ft=python:
