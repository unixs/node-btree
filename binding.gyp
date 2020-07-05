{
  # NOTE: 'module_name' and 'module_path' come from the 'binary' property in package.json
  # node-pre-gyp handles passing them down to node-gyp when you build from source
  "targets": [
    {
      "target_name": "glib",
      "direct_dependent_settings": {
        "include_dirs": [
          "src/include"
        ],
        'conditions': [
          ['OS == "linux"', {
            "include_dirs": [
              '<!@(pkg-config --cflags glib-2.0 | sed s/-I//g)'
            ],
            "libraries": [
              '<!@(pkg-config glib-2.0 --libs)'
            ]
          }]
        ]
      }
    },
    {
      "target_name": "array",
      "dependencies": [
        "glib"
      ],
      "sources": [
        "src/array.c"
      ]
    },
    {
      "target_name": "map",
      "dependencies": [
        "glib"
      ],
      "sources": [
        "src/map.c"
      ]
    },
    {
      "target_name": "specific",
      "dependencies": [
        "glib"
      ],
      "sources": [
        "src/specific.c"
      ]
    },
    {
      "target_name": "static",
      "dependencies": [
        "glib"
      ],
      "sources": [
        "src/static.c"
      ]
    },
    {
      "target_name": "iterators",
      "dependencies": [
        "glib"
      ],
      "sources": [
        "src/iterators.c"
      ]
    },
    {
      "target_name": "extra",
      "dependencies": [
        "glib"
      ],
      "sources": [
        "src/extra.c"
      ]
    },
    {
      "target_name": "<(module_name)",
      "sources": [
        "src/init.c"
      ],
      "dependencies": [
        "glib",
        "array",
        "map",
        "specific",
        "static",
        "iterators",
        "extra"
      ],
      'conditions': [
        ['OS == "linux"', {
          "include_dirs": [
            '<!@(pkg-config --cflags glib-2.0 | sed s/-I//g)'
          ],
          "libraries": [
            '<!@(pkg-config glib-2.0 --libs)'
          ]
        }]
      ],
    },
    {
      "target_name": "action_after_build",
      "type": "none",
      "dependencies": [
        "<(module_name)"
      ],
      "copies": [
        {
          "files": [ "<(PRODUCT_DIR)/<(module_name).node" ],
          "destination": "<(module_path)"
        }
      ],
      'conditions': [
        ['OS == "win"', {
            "copies": [
              {
                "files": [
                  "<(PRODUCT_DIR)/glib-2.dll",
                  "<(PRODUCT_DIR)/libcharset.dll",
                  "<(PRODUCT_DIR)/libiconv.dll",
                  "<(PRODUCT_DIR)/libintl.dll",
                  "<(PRODUCT_DIR)/pcre.dll"
                ],
                "destination": "<(module_path)"
              }
            ]
        }]
      ],
    }
  ]
}
