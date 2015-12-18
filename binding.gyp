{
    "targets": [
        {
            "target_name": "expand",
            "sources": [
                "src/expand.cc",
                "src/v8_utils.cc"
            ],
            "libraries": [
                "-lpostal"
            ]
        },
        {
            "target_name": "parser",
            "sources": [
                "src/parser.cc",
                "src/v8_utils.cc"
            ],
            "libraries": [
                "-lpostal"
            ]
        }
    ]
}
