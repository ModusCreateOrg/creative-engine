[![Modus Create](./images/modus.logo.svg)](https://moduscreate.com)
# Modus Create presents: creative-engine

This is a library of C++ classes for developing games for the ODROID-GO, the Raspberry Pi, and desktop Linux & MacOS.

The design criteria for this library/game engine are:
1) Portable
2) Host development using the CLion IDE and cmake and the SDL2 library
3) Run on device via cmake and Makefiles (make flash)
4) Games written using creative-engine should easily port to other devices - it already runs on macOS and Linux hosts, and Odroid-Go and Raspberry Pi 2+ targets.
5) Support for 2D and 3D games
6) Extendable through class inheritance

## Features
1) Double-buffered LCD display at 30Hz.
2) Resource management (and resource compiler).
3) Rich bitmap classes that support ROM and RAM pixel sources (resources).
4) Simple Sprites using sprite sheets.
5) Animated Sprites via animation interpreter, inherits from Simple Sprite.
6) State machines (Processes) for player and enemy logic.
7) Develop on host using rich development tools, run on target with little or no effort.

# Set up
See [the wiki](https://github.com/ModusCreateOrg/creative-engine/wiki) for instructions on getting things set up.

# Games made with creative-engine:
1) Genus

# ModusCreateOrg GitHub Guidelines

> Modus Create team members should refer to [ModusCreateOrg GitHub Guidelines](https://docs.google.com/document/d/1eBFta4gP3-eZ4Gcpx0ww9SHAH6GrOoPSLmTFZ7R8foo/edit#heading=h.sjyqpqnsjmjl)



