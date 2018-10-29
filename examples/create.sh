#!/usr/bin/env bash

# Set bash unofficial strict mode http://redsymbol.net/articles/unofficial-bash-strict-mode/
set -euo pipefail

if [ $# = 0 ]; then
    echo "Example name is required"
    echo "Usage: create.sh foobar"
    exit 1
fi

# Example names
NAME_LOWER=$(echo ${1} | tr '[:upper:]' '[:lower:]')
NAME_UPPER=$(echo ${1} | tr '[:lower:]' '[:upper:]')
# Credit to Stack Overflow https://stackoverflow.com/a/12487465/426540
NAME_CAPITALIZED="$(tr '[:lower:]' '[:upper:]' <<< ${1:0:1})${1:1}"

# Base example dir
# Credit to Stack Overflow questioner Jiarro and answerer Dave Dopson
# http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
# http://stackoverflow.com/a/246128/424301
BASE_DIR="$(cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
EXAMPLE_DIR=${BASE_DIR}/${NAME_LOWER}

# Create exaple dir
cp -r ${BASE_DIR}/template ${EXAMPLE_DIR}

# Replace stubs with example name provided
if [ "$(expr substr $(uname -s) 1 10)" == "MINGW32_NT" ]; then
    echo "32 bit Windows not supported"
    exit 1
elif [ "$(expr substr $(uname -s) 1 10)" == "MINGW64_NT" ]; then
    echo "64 bit Windows not supported"
    exit 1
elif [ "$(expr substr $(uname -s) 1 5)" == "Linux" ]; then
    sed -i -- "s/{stub}/${NAME_LOWER}/g" ${EXAMPLE_DIR}/CMakeLists.txt
    sed -i -- "s/{STUB}/${NAME_UPPER}/g" ${EXAMPLE_DIR}/CMakeLists.txt ${EXAMPLE_DIR}/Makefile
    sed -i -- "s/{Stub}/${NAME_CAPITALIZED}/g" ${EXAMPLE_DIR}/src/GSplashState.cpp
else
    # Assume BSD or MacOS
    sed -i '' -- "s/{stub}/${NAME_LOWER}/g" ${EXAMPLE_DIR}/CMakeLists.txt
    sed -i '' -- "s/{STUB}/${NAME_UPPER}/g" ${EXAMPLE_DIR}/CMakeLists.txt ${EXAMPLE_DIR}/Makefile
    sed -i '' -- "s/{Stub}/${NAME_CAPITALIZED}/g" ${EXAMPLE_DIR}/src/GSplashState.cpp
fi

echo "Successfully created '${NAME_LOWER}' example"
echo "Before building compile your resources with 'make resources'"
echo "Use CLion to build locally or 'make flash' for ODroid-GO"
