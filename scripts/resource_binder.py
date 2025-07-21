import os
import sys
import shutil
import string

# This script should take 3 extra arguments:
    # The resource folder to bind
    # A character, non-zero if you're on release mode and want to bind the resources into the exe, otherwise just copy to the output directory
    # The output directory, where the output folder should be located if on debug, or where the cpp output file will be generated


if len(sys.argv) < 4:
    raise Exception(f"Python script error ({__file__}): You need 3 arguments to run the script, you've only passed {len(sys.argv) - 1}")

resDir = sys.argv[1]
build_mode = int(sys.argv[2])
output_folder = sys.argv[3]

try:
        shutil.rmtree(output_folder + "/res")
except FileNotFoundError:
        pass

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
    
def buildresource():
     global out_string
     with open(output_folder + "/_res.c", "wb") as output_file:
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

        
        logfile = open(output_folder + "/_reslogs", "wb")
        logfile.write(out_string)
        logfile.seek(0, os.SEEK_SET)
        output_file.write(b"static const char s_game_res[] = {")
        for byte in out_string:
            output_file.write(str(byte).encode())
            output_file.write(b",")
        
        output_file.seek(-1, os.SEEK_CUR)
        output_file.write(b"}; const char* _game_res = s_game_res;")
        output_file.close()

        logfile.close()

if build_mode == 0:    
    shutil.copytree(resDir, output_folder + "/res")
else:
    success = False
    i = 0
    while not success:
        i += 1
        try:
           buildresource()
           success = True
        except PermissionError as p:
            pass
    print(i)
         
    

