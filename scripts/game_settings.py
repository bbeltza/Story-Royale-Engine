import json
import sys
from typing import Any
from webbrowser import get

from game_settings_types import types as settings_types

game_settings_path = sys.argv[1]
output_dir = sys.argv[2]
output_filedir = output_dir + "/g_settings.c"

output_header = '#include "GameSettings.h"\n'
output_header += 'const struct _sre_gs game_settings='

game_settings:dict = None
if game_settings_path != "0":
    with open(game_settings_path, "r") as game_settings_f:
        game_settings = json.load(game_settings_f)
else:
    game_settings = {}


def var_tocstr(val:dict | Any, types:list | Any, _key:str=None) -> str:
    result = ""
    if type(types) == list or type(types) == tuple:
        result += "{"
        
        assert len(types) > 0

        if type(types) == list:
            if val != None and type(val) != dict:
                raise TypeError(f"{_key} option must be a dictionary")
            for l in types:
                key = l[0]; assert type(key) == str
                default = l[1]
                result += var_tocstr(val.get(key, None) if val else {}, default, key)
                result += ","
        else:
            if val == None: val = types

            if not (type(val) == list or type(val) == tuple):
                raise TypeError(f"{_key} option must be a list or tuple")
            for i, v in enumerate(types):
                result += var_tocstr(val[i], v, _key)
                result += ","
            

        result = result.removesuffix(",") + "}"
    else:
        assert type(val) != dict, f"{_key} object must not be a dictionary"
        if val == None: val = types

        t = type(types)
        if t == str:
            assert type(val) == str, f"{_key} object must be a string, got {type(val)}"
            result = val.join(['"', '"'])
        elif t == bool:
            result = 'true' if val else 'false'
        else:
            result = str(val)
            if type(val) == str:
                # Assume the value is an enum
                result = "SRE_" + result

    return result

with open(output_filedir, "w") as outputfile:
    outputfile.write(output_header)
    struct_str = var_tocstr(game_settings, settings_types)
    struct_str += ";"
    outputfile.write(struct_str);
