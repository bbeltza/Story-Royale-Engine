from warnings import warn
import os, sys, typing, struct
import zlib

ARG_COUNT = len(sys.argv) - 1
REQUIRED_ARGS = 3

assert ARG_COUNT >= REQUIRED_ARGS, f"You need {REQUIRED_ARGS} or more arguments to run the script, you've only passed {ARG_COUNT}"

res_dir = sys.argv[1]
output_folder = sys.argv[2]
want_c_src = int(sys.argv[3]).__bool__()

try:
    ptr_size = int(sys.argv[4])
    if ptr_size <= 2:
        ptr_size = 2
    elif ptr_size < 4:
        ptr_size = 4
    elif ptr_size > 4:
        ptr_size = 8
except IndexError:
    ptr_size = sys.maxsize.bit_count() // 8 + 1

try:
    byte_order = "little" if sys.argv[5] == "LITTLE_ENDIAN" else "big" if sys.argv[5] == "BIG_ENDIAN" else None
    if byte_order == None: raise TypeError(f"Argument 5 must be either \"LITTLE_ENDIAN\" or \"BIG_ENDIAN\", you've passed \"{sys.argv[5]}\".")
except IndexError:
    byte_order = sys.byteorder

I = "I" if struct.calcsize("@I") == 4 else "L"
endianness_fmt = (">" if byte_order == "big" else "<")

offset_fmt = endianness_fmt + "B" + "x" * ptr_size
size_fmt = endianness_fmt + (I if ptr_size == 4 else "Q" if ptr_size == 8 else "H" if ptr_size == 2 else "N")

#compressor = zlib.compressobj(zlib.Z_NO_COMPRESSION, strategy=zlib.Z_FILTERED)

def getDirs(base:str, arr:list=[], prefix="") -> list:
    for dir in os.listdir(base + prefix):
        path = f"{prefix}/{dir}"
        fullpath = f"{base}{path}"
        if os.path.isdir(fullpath):
            getDirs(base, arr, path)
        elif os.path.isfile(fullpath):
            arr.append(path.removeprefix("/"))
        else:
            raise FileNotFoundError(path)
    
    return arr

def writeDirs(dirs:list[str], res_dat:typing.BinaryIO) -> list[int]:
    positions = []
    for path in dirs:
        res_dat.write(path.encode())
        positions.append(res_dat.tell() + 1)
        res_dat.write(struct.pack(offset_fmt, 0))
    return positions

def writeRes(base:str, dirs:list[str], res_dat:typing.BinaryIO):
    positions = writeDirs(dirs, res_dat)
    for i, fp in enumerate(dirs):
        fp = f"{base}/{fp}"
        pos = positions[i]
        rpos = res_dat.tell()
        res_dat.seek(pos, os.SEEK_SET)
        res_dat.write(struct.pack(size_fmt, rpos))
        res_dat.seek(0, os.SEEK_END)

        with open(fp, "rb") as resource:
            try:
                res_dat.write(struct.pack(size_fmt, os.path.getsize(fp)))
            except struct.error as err:
                raise MemoryError(f"I guess your resources are too big to support {ptr_size*8} bit files...")
            res_dat.write(resource.read())
        res_dat.write(b'\n')

def main():
    if not os.path.isdir(res_dir):
        raise FileNotFoundError(f"Resource path {res_dir} doesn't exist, or is not a directory.")

    dirs = getDirs(res_dir)
    with open(f"{output_folder}/_res.dat", "wb") as res_dat:
        writeRes(res_dir, dirs, res_dat)

        if want_c_src:
            # Convert raw data into C character array
            byte_array = []
            res_string = res_dat.read()
            for byte in res_string:
                b = hex(byte)
                byte_array.append(b)
            
            with open(f"{output_folder}/_res.c", "wt") as output_file:
                output_file.write("const unsigned char _game_res[]={")
                output_file.write(",".join(byte_array))
                output_file.write("};")

WANT_PROFILE = False

if WANT_PROFILE:  
    import cProfile
    import re
    cProfile.run('main()')
else:
    main()