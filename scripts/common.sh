#!/usr/bin/env bash

######################### Function definitions ########################

SUDO=${SUDO:-}

function ensure_xcode_installed {
    #Ensure XCode full version is installed and configured, 
    #as xcodebuild gets invoked later in the build, and it will fail otherwise
    if [[ -z "$(which xcodebuild)" ]] || ! xcodebuild --help >/dev/null 2>&1; then
        cat 1>&2 <<EOF
Please install XCode from the App Store.
You will need the full version, not just the command line tools.
If you already have XCode installed, you may need to issue this command
to let the tools find the right developer directory:
    "$SUDO" xcode-select -r
See https://github.com/nodejs/node-gyp/issues/569
EOF
        exit 1
    fi
}

function ensure_homebrew_installed {
    #Ensure homebrew is installed
    if [[ -z "$(which brew)" ]]; then
      echo "No homebrew found - installing Homebrew from https://brew.sh"
      /usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    fi
}

function ensure_cmake {
    # Adapted from https://askubuntu.com/questions/355565/how-do-i-install-the-latest-version-of-cmake-from-the-command-line
    local version
    local build
    local tmpdir
    local cmake
    version=3.12
    build=3
    tmpdir=$(mktemp -d)
    cmake="cmake-$version.$build-Linux-x86_64"
    cd "$tmpdir" || exit 1
    curl -sSO "https://cmake.org/files/v$version/$cmake.sh"
    $SUDO mkdir /opt/cmake
    yes | $SUDO sh "$cmake.sh" --prefix=/opt/cmake || true # exits 141 on success for some reason
    $SUDO rm -f /usr/local/bin/cmake
    $SUDO ln -s "/opt/cmake/$cmake/bin/cmake" /usr/local/bin/cmake
    rm -rf "$tmpdir"
}

function ensure_debian_devtools_installed {
    $SUDO apt-get -qq update
    $SUDO apt-get -qq install build-essential git libsdl2-dev libsdl2-image-dev curl
    # Ubuntu 18.04 has an old cmake (3.9) so install a newer one from binaries from cmake
    ensure_cmake
}

function ensure_arch_devtools_installed {
    $SUDO pacman -Sqyyu --noconfirm base-devel libglvnd sdl2 sdl2_image curl
    # Use same version of cmake as for ubuntu
    ensure_cmake
}

function ensure_creative_engine {
    if [[ ! -d "$CREATIVE_ENGINE_DIR" ]]; then
        git clone git@github.com:ModusCreateOrg/creative-engine.git "$CREATIVE_ENGINE_DIR"
    fi
}

function build {
    cd "$BASE_DIR" || exit 1
    if [[ ! -d creative-engine ]]; then
        rm -f creative-engine
        ln -s ../creative-engine . 
    fi
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR" || exit 1
    pwd
    ls -l
    ls -l ..
    cmake ..
    make
}

function clean {
    cd "$CREATIVE_ENGINE_DIR" || exit 1
    git clean -fdx
    rm -rf "$BASE_DIR/build"
}

# TODO: Use otool -L and some foo to find the dependencies
#		The sentinel is "/usr/local/opt"
function copy_sdl2_libs_to_app {
    if [ "$OS" == "Darwin" ]; then
        export APP_DIR="$BASE_DIR/build/genus.app"
        export APP_CNT_DIR="$APP_DIR/Contents"
        export APP_RES_DIR="$APP_CNT_DIR/Resources"
        export APP_MACOSX_DIR="$APP_CNT_DIR/MacOS"
        if [ -d $APP_DIR ]; then
        	rm -rf $APP_MACOSX_DIR/libs
        	mkdir -p $APP_MACOSX_DIR/libs

            cp /usr/local/opt/sdl2/lib/libSDL2.dylib $APP_MACOSX_DIR/libs/
            cp /usr/local/opt/sdl2_image/lib/libSDL2_image.dylib $APP_MACOSX_DIR/libs/
            cp /usr/local/opt/libpng/lib/libpng.dylib $APP_MACOSX_DIR/libs/
            cp /usr/local/opt/jpeg/lib/libjpeg.dylib $APP_MACOSX_DIR/libs/
            cp /usr/local/opt/libtiff/lib/libtiff.dylib $APP_MACOSX_DIR/libs/
            cp /usr/local/opt/webp/lib/libwebp.dylib $APP_MACOSX_DIR/libs/
            chmod 755 $APP_MACOSX_DIR/libs/*
         
            # FIX Genus
            install_name_tool -change \
            	/usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib \
             	./libs/libSDL2.dylib \
             	$APP_MACOSX_DIR/genus
            install_name_tool -change \
            	/usr/local/opt/sdl2_image/lib/libSDL2_image-2.0.0.dylib \
            	./libs/libSDL2_image.dylib \
            	$APP_MACOSX_DIR/genus
            
            # FIX SDL2_image
            install_name_tool -change \
            	/usr/local/opt/sdl2/lib/libSDL2-2.0.0.dylib \
            	./libs/libSDL2.dylib \
            	$APP_MACOSX_DIR/libs/libSDL2_image.dylib
            install_name_tool -change \
            	/usr/local/opt/libpng/lib/libpng16.16.dylib \
            	./libs/libpng.dylib \
            	$APP_MACOSX_DIR/libs/libSDL2_image.dylib
           	install_name_tool -change \
            	/usr/local/opt/jpeg/lib/libjpeg.9.dylib \
            	./libs/libjpeg.dylib \
            	$APP_MACOSX_DIR/libs/libSDL2_image.dylib
            install_name_tool -change \
            	/usr/local/opt/libtiff/lib/libtiff.5.dylib \
            	./libs/libtiff.dylib \
            	$APP_MACOSX_DIR/libs/libSDL2_image.dylib
            install_name_tool -change \
            	/usr/local/opt/webp/lib/libwebp.7.dylib \
            	./libs/libwebp.dylib \
            	$APP_MACOSX_DIR/libs/libSDL2_image.dylib

            # FIX TIFF
            install_name_tool -change \
            	/usr/local/opt/jpeg/lib/libjpeg.9.dylib \
            	./libs/libjpeg.dylib \
            	$APP_MACOSX_DIR/libs/libtiff.dylib

            # CREATE WRAPPER
            mv $APP_MACOSX_DIR/genus $APP_MACOSX_DIR/genus.bin
            tee $APP_MACOSX_DIR/genus <<-"EOF"
				#!/usr/bin/env bash
				MY_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")" && pwd )"
				(cd $MY_DIR && ./genus.bin)
				EOF
            chmod 755 $BASE_DIR/build/genus.app/Contents/MacOS/genus

            # INSTALL APP.PLIST & ETC
            cp $BASE_DIR/resources/info.plist $APP_CNT_DIR
            mkdir -p $APP_RES_DIR
            cp $BASE_DIR/resources/GenusIcon.icns $APP_RES_DIR

        fi
    fi
}

function checkout_creative_engine_branch {
    DEFAULT_BRANCH="master"
    GENUS_BRANCH=$(git branch | grep \* | cut -d ' ' -f2)
    echo "The current genus branch is: $GENUS_BRANCH"
    if (cd creative-engine && git checkout $GENUS_BRANCH); then
        echo "Checked out creatine-engine branch: $GENUS_BRANCH"
    elif (cd creative-engine && git checkout $DEFAULT_BRANCH); then
        echo "Checked out creatine-engine branch: $DEFAULT_BRANCH"
    else
        echo "Faied to checkout a branch for creatine-engine!"
        exit -1
    fi
}

