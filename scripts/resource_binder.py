from warnings import warn
import os
import sys
import zlib

ARG_COUNT = len(sys.argv) - 1
REQUIRED_ARGS = 3

assert ARG_COUNT >= 2, f"You need {REQUIRED_ARGS} arguments to run the script, you've only passed {ARG_COUNT}"

if ARG_COUNT > REQUIRED_ARGS:
    _args = sys.argv.copy()
    _args.pop(0)
    warn(f"You've passed more than {REQUIRED_ARGS} arguments. Only using the first {REQUIRED_ARGS} arguments, args: {_args}")

resDir = sys.argv[1]
output_folder = sys.argv[2]
want_c_src = int(sys.argv[3]).__bool__()

compressor = zlib.compressobj(zlib.Z_NO_COMPRESSION, strategy=zlib.Z_FILTERED)

def getTree(dir:str, array:list = []):
    for str in os.listdir(dir):
        try:
            newarray = [str]
            getTree(dir + f"/{str}", newarray)
            array.append(newarray)
        except NotADirectoryError:
            array.append((f"{str}"))
    return array

def write_dirs(_tree:list, prefix:str="", arr:list=[]):
    global out_string
    for filename in _tree:
           if type(filename) == str:
               arr.append([prefix + filename])
               out_string += arr[-1][0] + "\0001111"
               arr[-1].append(len(out_string)-4)

           elif type(filename) == list:
               if len(filename) == 0: continue
               pop = filename.pop(0) + "/"
               write_dirs(filename, prefix + pop, arr)
    return arr
    
def build_resource():
    global out_string
    tree = getTree(resDir)

    out_string = ""
    files = write_dirs(tree)
    out_string += "\n"
    out_string = out_string.encode()
    for filename in files:
        print(filename)
        with open(resDir + "/" + filename[0], "rb") as file:
            filedata = file.read()
            filesize = file.tell()
            print(f"filesize is {filesize}")

            sizechars = filesize.to_bytes(4)
            print(sizechars)
            pos = len(out_string)
            print(f"File position is {pos}")
            file_size_id = filename[1]

            out_string += sizechars
            out_string += filedata

            pos_to_bytes = pos.to_bytes(4)
            print(file_size_id)
            out_string = out_string[:file_size_id] + pos_to_bytes + out_string[file_size_id+4:]
                
            file.close()

        #out_compressed = compressor.compress(out_string)

    with open(output_folder + "/_res.dat", "wb") as logfile:
        logfile.write(out_string)
        logfile.seek(0, os.SEEK_SET)

    if want_c_src:
        # Convert raw data into C character array
        byte_array = []

        for byte in out_string:
            res = hex(byte)
            byte_array.append(res)
    
        with open(output_folder + "/_res.c", "wt") as output_file:
            output_file.write("const unsigned char _game_res[]={")
            output_file.write(",".join(byte_array))
            output_file.write("};")


def main():
    success = False
    i = 0
    while not success:
        i += 1
        try:
           build_resource()
           success = True
        except PermissionError as p:
            print(f'Something wrong happened when trying to open one of the files: {p.winerror}')
            pass

WANT_PROFILE = False

if WANT_PROFILE:  
    import cProfile
    import re
    cProfile.run('main()')
else:
    main()