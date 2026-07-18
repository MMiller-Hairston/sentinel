# C++ Project Starter Template

This is a quick-start project template for C++ projects which utilises a Core/App project architecture. There are two included projects - _Core_, and _App_. [Premake](https://github.com/premake/premake-core) is used to generate project files.

Core builds into a static library and is meant to contain common code intended for use in multiple applications. App builds into an executable and links the Core static library, as well as provides an include path to Core's code.

## Getting Started
1. Clone this repository or use the "Use this template" button on GitHub to quickly set up your own repository based on this template
2. `App/` and `Core/` are the two projects - you can edit the names of these folders and their contents to suit you
3. The three included Premake build files are `Build.lua`, `Core/Build-Core.lua` and `App/Build-App.lua` - you can edit these to customise your build configurations, edit the names of your projects and workspace/solution, etc.
4. Run `premake5 --cc=clang --file=Build.lua gmake` to generate project files for MacOS on Apple Silicone.

## Included
- Some example code (in `App/Source` and `Core/Source`) to provide a starting point and test
- Simple `.gitignore` to ignore project files and binaries

## License
- UNLICENSE for this repository (see `UNLICENSE.txt` for more details)
- Premake is licensed under BSD 3-Clause (see included LICENSE.txt file for more details)