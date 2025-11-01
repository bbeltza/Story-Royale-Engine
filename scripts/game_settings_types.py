types:dict = {
    "Title": ["My Game", "const char*"],
    "StartResolution": [[640, 480], "Vector2u"],
    "ScalingResolution": [[0, 0], "Vector2u"],
    "TargetFPS": [60, "uint16_t"],
    "WindowOptions": [[True, True, False], "WindowOptions", ["VSync", "Resizable", "Hidden"]],
    "AudioOptions": [[44100, 44100, 2, 0], "AudioOptions", ["InputFrequency", "OutputFrequency", "Channels", "Interpolation"]]
}

if __name__ == "__main__":
    from pprint import pprint
    pprint(types)
