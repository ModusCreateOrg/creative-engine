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

## Pre-requisites
On Mac:
```
$ brew install sdl2
$ brew install sdl2_image
```

On Linux:
```
$ sudo apt install libsdl2-dev libsdl2-image-dev
```

## Repository set up for development

1) FORK this repository using the fork button at GitHub.
2) clone your fork - do not clone this repository!
3) cd to your working directory
4) ```$ git remote add upstream git@github.com:ModusCreateOrg/creative-engine```
5) ```$ git fetch upstream```

To keep your master branch up to date with this repo:
```
$ git checkout master
$ git pull upstream master
```

When you work on an issue/ticket:
```
$ git checkout master
$ git pull upstream master # up to date!
$ git checkout -b branch-name  # branchname is ticket-number hyphen description
```

When you commit and push to your branch, you will have the opportunity to create a pull 
request from your fork against the official/repo.  

![test](images/image1.png?raw=true)

When you create a pull request, put a link to the GitHub issue in the first post, along 
with a description of what the PR does.

![test](images/image2.png?raw=true)

Note that GitHub automatically creates a link in the PR:

![test](images/image3.png?raw=true)

Next, go to the issue and put a link to the PR in a comment:
![test](images/image4.png?raw=true)

Or you can just hit # and select from the dropdown:
![test](images/image5.png?raw=true)

### NOTES
1) You will have to do the above steps for your game repository, as well.
2) Don't forget to change back to master branch after making your PR and doing the pull from upstream master
3) ALWAYS branch from master in your repo, unless you know what you're doing !!!

# ModusCreateOrg GitHub Guidelines

> Modus Create team members should refer to [ModusCreateOrg GitHub Guidelines](https://docs.google.com/document/d/1eBFta4gP3-eZ4Gcpx0ww9SHAH6GrOoPSLmTFZ7R8foo/edit#heading=h.sjyqpqnsjmjl)



