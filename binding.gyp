{
  "targets": [
    {
      "target_name": "wlc",
      "sources": [
        "src/wlcjs.cc",
        "src/interface.cc",
        "src/enum_to_string.cc",
        "src/output.cc",
        "src/view.cc",
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
