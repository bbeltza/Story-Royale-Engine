# The Story Royale Engine

Story Royale is, or will hopefully be a strategy RPG game based on Clash of Clans and The Last Guest. It is or is being made using a custom engine. This is the engine, a sort of SDL wrapper that uses entities and components to describe worlds/scenes, or GUI layers.

# Building

In this section, you'll be shown how to build the engine itself with the "tests" (they are more like examples, this will be changed with actual tests presented) included, and later, build an application/game with the engine attached.

## Requirements

In order to compile and build the engine, you must have:

- **CMake**: version 3.20 or higher
- A build program:
    - Usually **Visual Studio**'s solutions for *Windows*, or **Make** for *Linux*. I use **Ninja** which is available for both platforms.
- A compiler: **MSVC** for *Windows* and **GCC** for *Linux* are supported. **Clang** can be used but it is not often tested.
- **Python**: I'm not sure which version (really), but the latest version your package manager supports will do it.
    > Python is needed if you are willing to embed your game's resources, or/and to embed the icon for Windows. Otherwise, you can go without it. 
- An IDE or code editor is recommended, I mostly use **Visual Studio Code** to work on, but you can just use the terminal to build (which you'll be told how) and use notepad to code in.

- And finally, a copy to the engine, so you must have **git**. Although you can just download the .zip archive of the engine, you'll have to create a local repository and figure out how to import the required libraries (because libraies such as SDL will be shown as empty folders by just downloading this repository, that's how git's submodules work), so just use git, with the `--recursive` flag:
    ```sh
    git clone https://github.com/bbeltza/Story-Royale-Engine.git --recursive
    ```

## Building the engine

After meeting all of the requirements, and having a local copy of the engine, go and open that folder:

### Using an IDE with CMake integration

Some editors/IDE's (Like CLion, Visual Studio, or VScode with the CMake tools extension) provide full integration with CMake's ecosystem, by just a few clicks, you can set it up and manage to run it, it depends on what IDE you use, but by just opening the folder with the IDE it should automatically detect that it is a CMake project and prompt you to build it

There's also support for presets by a `CMakePresets.json` file, they allow you to group different options with configurations without having to type them from scratch, this engine has already some presets in it, but it is left abandoned, and I mostly use another `CMakePresets.json` file outside, so I encourage you to play around with it.

### Using the terminal

If your editor doesn't have built-in CMake integration, then you're going to want to use the terminal to build it. To start, go to the engine's folder in your terminal and start by configuring:
```sh
cmake -B build
```
You can also specify one of our presets by typing:
```sh
cmake --preset x64-debug
```
Now you have configured your project, but it not built yet. You have just created a configuration for the build program that you are using, build programs can be specified by using the `-G [generator]` flag [(Available options)](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html). The generated files are located in the `build` folder, you can change "build" in the previous command by whatever folder you want, but do not change it to a folder ***that already exists***. If you've used presets, the configuration files should be located at `out/build/[preset name]`.

There are multiple ways where you can build the project, you can either go to the generated folder and type the command (of your build program) that compiles it (For Visual Studio solutions there will be a .sln file there, just open it and build it with Visual Studio, you might need to setup the working directory if you're managing with resources). Or you can just use this cmake command in the engine's folder

```sh
cmake --build build # build or wherever your generated files are
```

Now, it should take a while to compile everything, and if everything is alright and there are no errors, the binaries should all be located at the `bin/` folder inside the earlier generated folder. Targets inside the `bin/` should have an executable in it. Note that you can specify a target when compiling the project, if you are not familiar with "targets", they're like separate projects in a workspace that may depend on other project in that same workspace. With cmake, use:

```sh
cmake --build build [Target Name] # This can be for example "FileTest" if you are building with the tests
```

## Building a game

Before reading this, I encourage you to see the ***Building the engine*** section, and to build the engine first, you have to be familiarized with this engine's build system before using it, reading those steps above is a nice way to do it, another way is to be familiarized with **CMake**, because you're going to want to write your own CMake scripts to make a game using this engine, it's not much, it's just specifying which source files to include and compile and then calling an integrated function, so you don't have to be an expert at CMake (By this I mean you don't have to understand the CMake inside the engine), but you'll have to understand what every line of **your** game's script does.

You'll also have to know some **C++** if you are willing to write a game with the engine, most of the engine was written in C++ (with some parts made in **C**) and the API is mostly made to be used with C++ (although a plain C API would be possible some day).

### Game structure

As of now, there are 2 ways in which you can place your game, you can either create your folder with the engine and the game, this also supports the ability to have multiple games with a single instance of the engine:

```sh
My Collection # Being the root directory where you configure CMake in build/
│   CMakeLists.txt
│
├───build
├───Engine # The engine
└───My Game # Your game
    │   CMakeLists.txt
    │
    └───src # All of your sources (.c, .cpp, ...)
        
```

> The `CMakeLists.txt` from the root directory should include both directories, like this:
>
>```cmake
>cmake_minimum_required(VERSION 3.12) # Normally this isn't required as you're only going to include folders, but some versions of CMake give you errors when not including this.
>
>add_subdirectory(Engine)
>add_subdirectory("My Game") # "My Game" can be anything, as well as "Engine", but of course it has to be the same name as the folder you're including.
>```

Or, you can just have your game as the root directory and place the engine inside:

```sh
My Game
│   CMakeLists.txt
│
├───Engine
└───src
```

But usually, it is better to have a collection where the engine is separated from the game for reasons discussed later.

### The CMakeLists.txt

This is a simple example of a `CMakeLists.txt` from the game:

```cmake
cmake_minimum_required(VERSION 3.12)
project(Game) # This can be anything, it's the name of the target

# Include this if you have the engine inside the game
# add_subdirectory(Engine)

include(StoryRoyaleEngine)

# Add all of your C/C++ files here
set(SOURCES "src/entry.cpp") # You can change the SOURCES name, back then, it was required that SOURCES existed
# If you don't want to add the sources manually, you can replace it with this line: (Note that you'll have to reconfigure your project every time you add a new source file, unless you also use the CONFIGURE_DEPENDS flag)
# file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

srEngine_property(Game PROPERTIES TITLE "My Game") # This function does add some additional data for your game, like the title...
                                                   # It is followed by `PROPERTIES` and then anything that you can add in this pattern: [NAME] <VALUE> ...
                                                   # There are currently two more supported properties:
                                                        # ICONS: Speficy where the icons are stored
                                                        # RESOURCES: Speficy where the resources are stored


srEngine_build(Game GameExecutable SOURCES) # This function does all of the heavylifting. Taking your SOURCES variable as the third argument and making an executable out of it from the second one, with the engine linked. There are some options though that you can add as arguments:
# NO_CONSOLE uses a Windows subsystem on Windows, that just means it doesn't include a console
# NO_BIND is used to not embed the game assets into the executable, instead it will just put them into a single "__res/" folder in the binary's folder, this is useful for debugging as embedding resources takes some time.
  # It was more useful back then when embedding resources took way more.
# RES_DEBUG can be used to point your resources to the source directory's original resources, and not make a copy of it. This allows for changing your assets without having to rebuild the game, but it is meant for debugging, do not expect to be able to run the .exe on another computer with that option

```

### Writing a game

After you've configured everything, you're going to have to write a **C/C++** file with a `sre::initialize()` function in it. It is required, otherwise you'll get a linking error:

```c++
// Include runtime features of the engine
#include <Core/Runtime.hpp>
#include <Core/Window.hpp>
#include <Base/Log.h> // Include logging
#include <Entry.h> // Include the entry point declaration (sre::initialize())

void myUpdateFunction()
{
    sre::log("My framerate is: %g", 1 / sre::dt); // Print the framerate into the console
                                            // Should be around 39.-- if you set the framerate to 40
}

void sre::initialize()
{
    sre::window_setresizable(true); // Most function names in this API follow a similar pattern:
                                        // The topic of the function (in this case "window") followed by an underscore
                                        // And the action of the function ("setresizable" in this case)
                                    // Call this with `true` if you want to have some fun resizing your window...

    sre::log("Hello World!");
    sre::onUpdate.connect(myUpdateFunction, NULL); // Connect myUpdateFunction to a Signal that will be run every frame
    // Signals are objects that store functions to be called when fired 
    // sre::onUpdate is a signal that fires once every frame
    // The parameter `NULL` is for an address passed by the connection, in this case, we do not need it.
}
```

>This is what a simple program with this engine can be, there are essential features such as the *ECS* (Entity Component System) that aren't used there, and can be used to define the game. But more on that later...

Once you've compiled your source file, you should be able to see a resizable window with `Hello World!` on the console and get `My framerate is: xx.xx` printed on every frame (assuming you didn't use the NO_CONSOLE flag on Windows)

#

>More documentation will be available later, I'm here if you require any help
