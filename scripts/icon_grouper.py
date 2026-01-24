from sys import argv
from shutil import copyfile
from os import mkdir
import image_helpers

def group_icons(output_folder: str, icons: list[str]):
    try:
        mkdir(output_folder)
    except FileExistsError:
        pass
    
    for icon_path in icons:
        dimensions = image_helpers.getpng_dimensions(icon_path)
        if not dimensions: continue

        x, y = dimensions
        output_path = output_folder + f"/{x}x{y}"

        copyfile(icon_path, output_path)
        ###
        


def main():
    argv.pop(0)
    argc = len(argv)
    if argc < 2:
        raise TypeError("Not enough arguments: icon_grouper.py output_folder [icons...]")
    
    output_folder = argv[0]
    argv.pop(0)

    group_icons(output_folder, argv)


if __name__ == "__main__":
    main()