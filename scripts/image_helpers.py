from os import SEEK_SET, SEEK_CUR, SEEK_END
import struct

BMP_SEEK_OFFSET: int = 0x12
PNG_SEEK_OFFSET: int = 0x10
BMP_HEADER: bytes = b"\x42\x4D"
PNG_HEADER: bytes = b"\x89\x50\x4E\x47\x0D\x0A\x1A\x0A"

def getpng_dimensions(path: str) -> tuple[int, int] | None:
    with open(path, "rb") as file:
        header = file.read(8)
        if header != PNG_HEADER:
            return None
        
        if file.seek(PNG_SEEK_OFFSET, SEEK_SET) != PNG_SEEK_OFFSET:
            return None
        return struct.unpack(">II", file.read(8))
    
def getbmp_dimensions(path: str) -> tuple[int, int] | None:
    with open(path, "rb") as file:
        header = file.read(2)
        if header != BMP_HEADER:
            return None
        
        if file.seek(BMP_SEEK_OFFSET, SEEK_SET) != BMP_SEEK_OFFSET:
            return None
        return struct.unpack("<II", file.read(8))

def test_helpers():
    from sys import argv
    for i in range(1, len(argv)):
        print(getpng_dimensions(argv[i]))
        print(getbmp_dimensions(argv[i]))

if __name__ == "__main__":
    test_helpers()