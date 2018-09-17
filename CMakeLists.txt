cmake_minimum_required(VERSION 3.9)
project(creative_engine)

set(CMAKE_CXX_STANDARD 11)

include_directories(include)

add_executable(creative_engine
        images/modus.logo.svg
        lib/BAnimSprite.h
        lib/BBase.h
        lib/BBitmap.h
        lib/BGameEngine.h
        lib/BList.h
        lib/BPlayfield.h
        lib/BProcess.h
        lib/BResourceManager.h
        lib/BSoundPlayer.h
        lib/BSprite.h
        lib/BTypes.h
        lib/BViewPort.h
        lib/Controls.h
        lib/Display.h
        lib/GameEngine.h
        lib/Panic.h
        lib/BAnimSprite.cpp
        lib/BBase.cpp
        lib/BBitmap.cpp
        lib/BGameEngine.cpp
        lib/BList.cpp
        lib/BPlayfield.cpp
        lib/BProcess.cpp
        lib/BResourceManager.cpp
        lib/BSoundPlayer.cpp
        lib/BSprite.cpp
        lib/BTypes.cpp
        lib/BViewPort.cpp
        lib/Controls.cpp
        lib/Display.cpp
        lib/Panic.cpp
        CODE_OF_CONDUCT.md
        CONTRIBUTING.md
        LICENSE
        README.md)
