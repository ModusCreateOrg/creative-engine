[![Modus Create](./images/modus.logo.svg)](https://moduscreate.com)
# creative-engine

This is a library of C++ classes for developing games for the Odroid-Go.

The design criteria for this library/game engine are:
1) Portable
2) Host development using CLion and SDL (Cmake)
3) Run on host via Makefiles (make flash)
4) Games written using creagive-engine should easily port to other devices - it already runs on host and Odroid-Go.
5) Support for 2D and 3D games
6) Estendable through class inheritance

## Features
1) Double buffered LCD display at 30Hz.
2) Resource management (and resource compiler).
3) Rich bitmap classes that support ROM and RAM pixel sources (resources).
4) Simple Sprites using sprite sheets.
5) Animated Sprites via animation interpreter, inherits from Simple Sprite.
6) State machines (Processes) for player and enemy logic.
7) Devlop on host using rich development tools, run on target with little or no effort.

## Repository set up for development

1) FORK this repository using the fork button at GitHub.
2) clone your fork - do not clone this repository!
3) cd to your working directory
4) ```$ git remote add upstream git@github.com:ModusCreateOrg/creative-engine```
5) ```$ git fetch upstream```

To keep your master branch up to date with this repo:
```
$ git checkout master
$ git merge upstream/master
```

When you work on an issue/ticket:
```
$ git checkout master
$ git merge upstream/master # up to date!
$ git checkout -b branch-name  # branchname is ticket-number hyphen description
```

When you commit and push to your branch, you will have the opportunity to create a pull 
request from your fork against the official/upstream repo.  

When you create a pull request, put a link to the GitHub issue in the first post, along 
with a description of what the PR does.


# ModusCreateOrg GitHub Guidelines
This repository serves as a collection of useful scripts, configuration files, and settings for GitHub repositories.

> Modus Create team members should refer to [ModusCreateOrg GitHub Guidelines](https://docs.google.com/document/d/1eBFta4gP3-eZ4Gcpx0ww9SHAH6GrOoPSLmTFZ7R8foo/edit#heading=h.sjyqpqnsjmjl)


[image1](images/image1.png)

