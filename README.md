# Blackhole Package Manager

## Overview
Blackhole is a simple, minimalistic package manager for Linux-like systems. It is designed to manage software packages with ease, providing a foundation for handling package installation, removal, and management tasks in a straightforward manner.

## Features
- **Minimalistic design** for easy use and integration
- Support for **custom repository** management
- Configurable **cache** directory for storing **package** metadata
- Color output in terminal for better readability
- **Package dependency** handling (OPTIONAL, REQUIREMENT, CONFLICT)
- The configuration files are in json format

## Dependencies
- CMake (for building the project)
- nlohmann_json 3.2.0 or higher
- libcurl (for downloading packages and repository information)
- OpenSSL

## Building from Source
To build *blackhole* from source, follow these steps:

1. **Clone the repository:**
```bash
git clone https://github.com/fszontagh/blackhole.git
cd blackhole
```

2. **Prepare the build environment:**
```bash
mkdir build && cd build
cmake ..
```
Available configure options:
- `BUILD_TESTS = ON ` enable / disable tests on build time
- `BUILD_CREATOR = ON` enable / disable the package creator component
- `ENABLE_COLORS = ON` enable / disable terminal colors when no logfile is used
- `CONFIG_DIRECTORY = "/etc/blackhole"` where the main and user related configuration files stored
- `CONFIG_DOT_DIRECTORY = "conf.d"` where the user related configutation files stored

With defaults settings, the configuration files will be searched in the `/etc/blackhole/conf.d` directory.

4. **Build the project:**
```Bash
cmake --build .
```

The executable(s) will be located in the `build` directory.

## Configuring Blackhole
The main configuration for Blackhole is handled through a JSON configuration file. You need to specify the repository URLs and the cache directory.

Example configuration format:
```json
{
"repositories": [
 {
   "url": "http://example.com/repo",
   "configFile": "repo.json"
 }
],
"cacheDir": "/var/cache/blackhole"
}
```