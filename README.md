# Network shooter project #

This is a C++ game engine for creating tile-based realtime 2D games. For example, top-down 2D shooters. It's based on Box2D and SFML.

### Features ###

* Enables you to create multiplayer games.
* Crossplatform.
* Uses MVC pattern.
* Contains self-written network library.
* Contains gui library and easy-to-use scene manager.
* Supports [Tiled map editor](http://www.mapeditor.org/).
* Uses components to construct game characters.
* All network communications are done via UDP protocol.

### How to build ###
1. Download and install [CMake](https://cmake.org/).
2. Download [SFML](http://www.sfml-dev.org/download.php) or [build it from the source](http://seriousitguy.blogspot.ru/2014/05/how-to-setup-sfml-on-ubuntu-1404-lts.html).
3. Get [boost](http://www.boost.org/) headers. *Note, that you don't have to build boost, because the project depends just on header-only libraries.*
4. Download and build [Box2D](https://github.com/erincatto/Box2D).
5. Configure CMakeLists.txt to select folders with libraries.
6. Run CMake and build the project.

### How to run ###
1. Place /res folder from the repository root near the binaries.
2. Place SFML dynamic libraries near the binaries.

### Running the client without all resources ###

The client doesn't need to store all tile images. They will be downloaded. It also doesn't need to store levels and properties.xml. But it won't work, if it doesn't have these files and directories:

1. **res/graphic/legs2** - *it may be empty*
2. **res/graphic/default.png** - *it is used as a default tile*
3. **res/gui** - *all files from this directory are used to render the gui*