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
    > Python is required to run some scripts that include generating the game's settings and assets. I might reduce Python's requirements and use dummy sources if the user doesn't have it.
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

There are multiple ways where you can build the project, you can either go to the generated folder and type the command (of your build program) that compiles it (For Visual Studio solutions there will be a .sln file there, just open it and build it with Visual Studio). Or you can just use this cmake command in the engine's folder

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
    │   GameSettings.json
    │
    └───src
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
│   CMakeLists.txt # We will discuss about these two files next
│   GameSettings.json # Both files allow you to configure the game
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
# Some errors occur when you're building inside the engine due to the policies (they're like rules) not being set outside, if it occurs, try including this macro:
# srEngine_policy()

# Add all of your C/C++ files here
set(SOURCES "src/entry.cpp") # Note that SOURCES has to remain, this variable is used for the next line
# If you don't want to add the sources manually, you can replace it with this line: (Note that you'll have to reconfigure your project every time you add a new source file, unless you also use the CONFIGURE_DEPENDS flag)
# file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")

srEngine_build(Game) # This function does all of the heavylifting. Taking SOURCES and making an executable out of it, with the engine linked. There are some options though that you can add as arguments:
# NO_CONSOLE uses a Windows subsystem on Windows, that just means it doesn't include a console
# NO_BIND is used to not embed the game assets into the executable, instead it will just put them into a single "__res/" folder in the binary's folder, this is useful for debugging as embedding resources takes some time.
# It was more useful back then when embedding resources took way more.

```

This is all you normally need to build a game with this engine, the other part can be done with the `GameSettings.json` file.

### The GameSettings.json

These are just settings! It's just a **JSON** file with hints to the engine on how to start your application. You don't have to modify any of your CMake if you want to add it, the next time you reconfigure your project it will automatically detect the file, that means `GameSettings.json` cannot have a different name. Here's an example with all of the available options:

```jsonc
{
    "Title": "My Game", // The title of the game, this usually sets up the title of the window and the name of the local user's data folder
    "StartResolution": [800, 600], // The start resolution of the window, pretty simple, can be [0, 0] if you've already specified a scaling resolution which comes next
    "ScalingResolution": [320, 180], // The target scaling resolution, it's just for pixel-art games to scale in, you can completely remove if if you want a native resolution
    "TargetFPS": 60, // The target FPS limits of the game, can be 0 if you don't want any limits
    "WindowOptions": {
        "VSync": true, // Pretty simple, VSync
        "Resizable": true, // If the window can be resized or not
	    "Hidden": false // If you don't want the window to automatically show up. This is enabled in Story Royale to load first scene and then show the window 
    },
    "AudioOptions": {
        "Frequency":  44100, // The frequency of the audio
        "Filter": "Linear" // This isn't really used, it specifies the way you want to interpolate the audio if for example a song doesn't use the same frequency as the frequency you've specified.
        // But it is unavailable and defaults to a linear interpolation for now
    },
    // Note that these two last options are used by cmake, so you'll have to reconfigure it every time you change it
    "Resources": "res", // The folder which you want your resources to be in
    "Icon": "icon" // The folder that contains all of the different icons (must be either .bmp or .png) depending on the size
}
```

> *You don't have to add every single option in them, there are default values for the options that you did not set*

### Writing a game

After you've configured everything, you're going to have to write a **C/C++** file with a `Game::Initialize()` function in it. It is required, otherwise you'll get a linking error:

```c++
#include <Engine.hpp> // Include some base feature of the engine
#include <utils/logging.h> // Include LOG(), it's just a printf() wrapper

void Game::Initialize()
{
    LOG("Hello World!");

    Window::show();
}
```

>This is the most simple a program with this engine can get, there are essential features such as *Worlds* and *Entities* that aren't used there, and can be used to define the game. But more on that later...

Once you've compiled your source file, you should be able to see `Hello World!` on the console (assuming you didn't use the NO_CONSOLE flag on Windows)

#

>More documentation will be available later, I'm here if you require any help
