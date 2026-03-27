# CMP425 Coursework

## Building

Build with [CMake](https://cmake.org/) 3.28 or later.
The configure step will automatically fetch all the required dependencies via git.

> [!NOTE]
> It should work with any compiler, but I switched over to using Clang for the last third of the project so some unforseen MSVC compatibility issues may have crept in since then.
> 
> Probably stick to the pre-built binaries.

The Clang build needs access to a few dlls - `libgcc_s_seh-1.dll`, `libstdc++-6.dll` and `libwinpthread-1.dll` - in order to run. If you're working with MSYS2, these will be on your path; remember to bundle them, and the resources, into builds before distribution.

## Running

Command line arguments can be used to set a number parameters, listed below.

All are optional and will take on reasonable defaults if not set, so they don't have to be run from the command line in order to function.

| Option | Description | Fallback |
|:-:|-|:-:|
| `--resource-dir {path}` | Path to the resources folder. | `resources/` |
| `--user-dir {path}` | Path to the user data folder, where saves go. | `user/` |
| `--interpolation {type}` | Interpolation mode to use. | `default` |
| `--no-debug`, `--ndebug` | Disable debug output. | `false` |
| `--no-ansi` | Disable ANSI formatting in output. | `false` |
| ***Client-only*** | | |
| `--user {val}`, `-u {val}` | Username to connect to the server under. | Connect as guest |
| `--observer`, `-o` | Do not create a player for this client. | |
| `--server {ip}`, `-s {ip}` | Ip address at which to seek the server. | `LocalHost` |
| ***Server-only*** | | |
| `--world {name}`, `-s {ip}` | Name of world to open. | `fallback_world` |

> [!NOTE]
> Both the client and server need access to the files in the `resources` folder, which they will seek relative to their working directory unless overridden with `--resource_dir`.
> 
> This means that the exes in the build folder will crash if they are run directly from the file explorer (the VSCode launch configuration launches them with their working directory set to the project root).

### Controls

`WASD` to move, `Space` to jump, `LCtrl` to run.

`LMB` to place blocks, `RMB` to break blocks.

#### Debug

| Keybind | Description |
|:-:|-|
| `Y` | Toggle tick counter hud. |
| `T` | Toggle tilemap debug hud. |
| `N` | Toggle network debug hud. |
| `H` | Toggle actor debug hud. |
| ***Interpolation*** | |
| `I` | Switch to next interpolation mode. |
| `Shift + I` | Switch to previous interpolation mode. |
| `O` | Switch to default interpolation mode. |
| `K` | Disable interpolation. |

