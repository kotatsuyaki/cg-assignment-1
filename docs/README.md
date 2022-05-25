<!-- vim: set ft=markdown.pandoc colorcolumn=100: -->

# Project Overview

This project is an overengineered version of the original assignment template.
While it may not seem like the original template project at all,
it was indeed incrementally refactored from the template.
Most of the code are abstractions and wrappers around the underlying raw APIs.


# Usage

## Model Folder Picker

A folder picker dialog[^1] is implemented for the user to select the folder containing model files to load
(see [screenshots](#screenshots) for reference).
All files with the `.obj` extension directly under the path selected is loaded into the program.
Material files with the `.mtl` extension is also searched for under the same path.

The model files used for testing can [be found here](https://github.com/kotatsuyaki/nthu-cg-models).

## Key Mappings

All key mappings specified in the assignment spec are implemented.

| Key     | Function                                   |
|---------|--------------------------------------------|
| `w`     | Toggle wireframe                           |
| `z`     | Previous model                             |
| `x`     | Next model                                 |
| `o`     | Activate orthogonal projection mode        |
| `p`     | Activate perspective projection mode       |
| `t`     | Set control to translation mode            |
| `s`     | Set control to scale mode                  |
| `r`     | Set control to rotation mode               |
| `e`     | Set control to eye position mode           |
| `c`     | Set control to viewing center mode         |
| `u`     | Set control to camera up vector mode       |
| `i`     | Print debug information to standard output |
| `l`     | Cycle among the lighting modes             |
| `k`     | Set control to light editing mode          |
| `j`     | Set control to shininess editing mode      |
| `v`[^2] | Toggle VSync (defaults to true)            |

[^1]: This is not part of the assignment spec.
    It was introduced to avoid hard-coding path to the object files.
[^2]: This is not part of the assignment spec.
    It was introduced to prevent my laptop battery from dying when running the binary.


# Dependencies

- **C++17-compliant compiler**
- CMake
- GNU Make and pkg-config (Linux and macOS only)
- pkg-config, D-Bus, Xorg (Linux only)


# Build from Source

This project is tested on GitHub Actions to ensure that it builds successfully on all major platforms.
Below subsections list out build instructions for Linux, Windows, and macOS.
For detailed commands used on GitHub Actions, see `.github/workflows/build.yml`.

## Linux

Tested on Ubuntu 20.04.

1. Install dependencies.

    ```sh
    apt-get install -y g++ cmake make pkg-config libdbus-1-3 libdbus-1-dev xorg-dev
    ```
2. Generate makefile.

    ```sh
    cmake -DCMAKE_BUILD_TYPE=Release .
    ```
3. Run `make`.

    ```sh
    make -j
    ```

The resulting binary is `proj`.

## Windows

Tested on Visual Studio 16 and later.
The *Desktop development with C++* workload in Visual Studio comes with CMake pre-installed,
available for use from the *Developer PowerShell for Visual Studio*.

1. Generate Visual Studio project.

    ```sh
    cmake -G "Visual Studio 16 2019" -A x64 .
    ```

2. Build the project.  The resulting binary is `Release/proj.exe`.

    ```sh
    msbuild proj.sln /p:Configuration=Release
    ```

Alternatively it's also possible to open the solution file with the Visual Studio IDE.
To run the program from the IDE, set `proj` as the startup project from the solution explorer.

## macOS

Tested on Catalina and later.
macOS versions earlier than that probably does not work,
since Apple did not backport C++17 support to them.

1. Generate makefile.

    ```sh
    cmake -DCMAKE_BUILD_TYPE=Release .
    ```
2. Run `make`.

    ```sh
    make -j
    ```

Alternatively it is also possible to generate an Xcode project by running `cmake -G Xcode .`.

It's a known problem that bare unix executable can't get window focus and thus can't receive keyboard controls.
An easy fix is to put the binary inside a manually created app bundle like this.

```sh
# Create app bundle
mkdir -p App.app/Contents/MacOS/
# Put the unix executable into the bundle
cp <path to binary> App.app/Contents/MacOS/App
```


## NixOS / Nix Package Manager

Easy, just do

```sh
nix build .#
```

The binary is at `./result/bin/proj`.


# Screenshots

![The model file folder picker](./screenshots/folder-picker.png)

![The rabbit model in default setup with directional light](./screenshots/rabbit-default-directional.png)

![The rabbit model in default setup with point light](./screenshots/rabbit-default-point.png)

![The rabbit model in default setup with spotlight](./screenshots/rabbit-default-spotlight.png)

![The rabbit model with directional light from the left](./screenshots/rabbit-from-left-directional.png)

![The rabbit model with spotlight focusing on the head](./screenshots/rabbit-head-spotlight.png)
