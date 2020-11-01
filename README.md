# Lab 4 myrls

## Team

 - [Mykyta Samovarov](https://github.com/Voravomas)

## Prerequisites

 - **C++ compiler** - needs to support **C++17** standard
 - **CMake** 3.15+
 
The rest prerequisites (such as development libraries) can be found in the [packages file](./apt_packages.txt) in the form of the apt package manager package names.

## Installing

1. Clone the project.
    ```bash
    git clone git@github.com/Voravomas/myrls
    ```
2. Install required packages. On Ubuntu:
   ```bash
   [[ -r apt_packages.txt ]] && sed 's/\r$//' apt_packages.txt | sed 's/#.*//' | xargs sudo apt-get install -y
   ```
3. Build.
    ```bash
    cmake -Bbuild
    cmake --build build
    ```

## Usage

```myrls
myrls
myrls ../
```

Description:
	This program recursively prints content
	of a file or a directory.
	Only one path is allowed.
Allowed options:
  -h [ --help ]         Produce help message
  --input-file arg      Enter file or directory