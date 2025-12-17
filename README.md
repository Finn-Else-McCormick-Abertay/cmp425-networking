**CMP425 Coursework** - Finn Else-McCormick 2200864

## Building

Build with [CMake](https://cmake.org/) 3.28 or later.
The configure step will automatically fetch all the required dependencies via git.

> [!NOTE]
> It should work with any compiler, but I switched over to using Clang for the last third of the project so some unforseen MSVC compatibility issues may have crept in since then.
> Probably stick to the pre-built binaries.

## Running

Command line arguments can be used to set a number parameters, listed below.
All are optional and will take on reasonable defaults if not set, so they don't have to be run from the command line in order to function.

| Option | Description | Fallback |
|-|-|-|
| `--resource_dir {path}` | Path to the resources folder. | `resources/` |
| `--user_dir {path}` | Path to the user data folder, where saves go. | `user/` |
| **Client-only** | | |
| `--user {val}`, `-u {val}` | Username to connect to the server under. | Connect as guest |
| `--server {ip}`, `-s {ip}` | Ip address at which to seek the server. | `LocalHost` |
| **Server-only** | | |
| `--world {name}`, `-s {ip}` | Name of world to open. | `fallback_world` |

> [!NOTE]
> Both the client and server need access to the files in the `resources` folder, which they will seek relative to their working directory unless overridden with `--resource_dir`.
> This means that the exes in the build folder will crash if they are run directly from the file explorer (the VSCode launch configuration launches them with their working directory set to the project root).