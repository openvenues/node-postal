{
    "targets": [
        {
            "target_name": "expand",
            "sources": [
                "src/expand.cc"
            ],
            "libraries": [
                "-lpostal", "-L/usr/local/lib"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "/usr/local/include"
            ]
        },
        {
            "target_name": "parser",
            "sources": [
                "src/parser.cc"
            ],
            "libraries": [
                "-lpostal", "-L/usr/local/lib"
            ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")",
                "/usr/local/include"
            ]
        }
    ]
}
