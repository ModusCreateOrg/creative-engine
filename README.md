[![Modus Create](./images/modus.logo.svg)](https://moduscreate.com)
# creative-engine

This is a library of C++ classes for developing games for the Odroid-Go.

The design criteria for this library/game engine are:
1) Portable
2) Host development using CLion and SDL (Cmake)
3) Run on host via Makefiles (make flash)
4) Games written using creative-engine should easily port to other devices - it already runs on host and Odroid-Go.
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
See the wiki for instructions on getting things set up.

# Games made with creative-engine:
1) Genus

# ModusCreateOrg GitHub Guidelines

> Modus Create team members should refer to [ModusCreateOrg GitHub Guidelines](https://docs.google.com/document/d/1eBFta4gP3-eZ4Gcpx0ww9SHAH6GrOoPSLmTFZ7R8foo/edit#heading=h.sjyqpqnsjmjl)



