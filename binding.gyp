{
    "targets": [
        {
            "target_name": "find-circles",
            "sources": [ "find-circles.cpp" ],
            "include_dirs": [
                "<!(node -e \"require('nan')\")"
            ]
        }
    ]
}
