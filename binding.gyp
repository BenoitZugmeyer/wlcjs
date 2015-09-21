{
  "targets": [
    {
      "target_name": "wlc",
      "sources": [
        "src/wlcjs.cc",
        "src/interface.cc",
        "src/enum_to_string.cc",
        "src/keysym_to_string.cc",
      ],
      "libraries": [
        "-lwlc"
      ]
    }
  ]
}
