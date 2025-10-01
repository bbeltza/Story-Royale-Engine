import json
import sys

from game_settings_types import types as settings_types

game_settings_path = sys.argv[1]
output_dir = sys.argv[2]
output_filedir = output_dir + "/g_settings.cpp"

output_header = '#include "GameSettings.hpp"\n'
namespace_prefix = " const GameSettings::"

def to_c_assign(value, defaultval) -> str:
    t = type(value)

    if t == str:
        if type(defaultval) != t: return str(value)
        return f"\"{value}\""
    elif t == bool:
        return value and "true" or "false"
    elif t == list:
        out_str = "{"
        for i, _ in enumerate(defaultval):
            out_str += to_c_assign(value[i], defaultval[i])
            if i < len(defaultval) - 1: out_str += ","
        
        return out_str + "}"
    else:
        return str(value)

game_settings:dict = None
if game_settings_path != "0":
    with open(game_settings_path, "r") as game_settings_f:
        game_settings = json.load(game_settings_f)
else:
    game_settings = {}

with open(output_filedir, "w") as outputfile:
    outputfile.write(output_header)

    for k, v in settings_types.items():
        ctype = v[1]
        assign_str = ctype + namespace_prefix + k + "="

        value = game_settings.get(k, v[0])

        if type(value) == dict:
            l = []
            for key in v[2]:
                l.append(value.get(key, v[0][len(l)]) )
            
            value = l

        assign_str += to_c_assign(value, v[0])
        
        assign_str += ";\n"
        print(assign_str)
        outputfile.write(assign_str)