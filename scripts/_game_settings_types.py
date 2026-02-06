types: list = [
    ["Title", "My Game"],
    ["WindowOptions", [
        ["Resolution", (640, 480)],
        ["TargetFPS", 60],
        ["VSync", True],
        ["Resizable", True],
        ["Scaled", False]
        ]
     ],
     ["AudioOptions", [
         ["Frequency", 0],
         ["Filter", 0],
         ["Mono", False]
         ]
      ]
]

if __name__ == "__main__":
    from pprint import pprint
    pprint(types)
