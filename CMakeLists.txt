cmake_minimum_required(VERSION 3.9)
project(creative_engine)

set(CMAKE_CXX_STANDARD 11)



add_library(creative_engine
        src/BAnimSprite.h
        src/BBase.h
        src/BBitmap.h
        src/BGameEngine.h
        src/BList.h
        src/BPlayfield.h
        src/BProcess.h
        src/BResourceManager.h
        src/BSprite.h
        src/BTypes.h
        src/BViewPort.h
        src/Controls.h
        src/Display.h
        src/GameEngine.h
        src/Panic.h
        src/BAnimSprite.cpp
        src/BBase.cpp
        src/BBitmap.cpp
        src/BGameEngine.cpp
        src/BList.cpp
        src/BPlayfield.cpp
        src/BProcess.cpp
        src/BResourceManager.cpp
        src/BSprite.cpp
        src/BTypes.cpp
        src/BViewPort.cpp
        src/Controls.cpp
        src/Display.cpp
        src/Panic.cpp
        CODE_OF_CONDUCT.md
        CONTRIBUTING.md
        LICENSE
        README.md)
