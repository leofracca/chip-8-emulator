# Instructions

These instructions explain how to set up the tools required to build the emulator.

## Linux

### Fedora

To install the software required:

```shell
sudo dnf install make cmake g++ git SDL2 SDL2-devel
```

### Other distros

TODO.

## Windows

TODO.

## MacOS

TODO.

## Build the emulator

Now that you have installed all the required software you can build the emulator:

```shell
git clone https://github.com/leofracca/chip-8-emulator
cd 
mkdir build
cd build
cmake ..
make
```
