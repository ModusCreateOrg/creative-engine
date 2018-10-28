#!/usr/bin/env bash
# Because Docker hates symlinks we need to write a wrapper script
# to run Docker from the TOP_DIR directory instead. Grr.

######################### Debugging and Directory Scaffolding #########

# Set bash unofficial strict mode http://redsymbol.net/articles/unofficial-bash-strict-mode/
set -euo pipefail
IFS=$'\n\t'

# Enable for enhanced debugging
#set -vx

# Credit to https://stackoverflow.com/a/17805088
# and http://wiki.bash-hackers.org/scripting/debuggingtips
export PS4='+(${BASH_SOURCE}:${LINENO}): ${FUNCNAME[0]:+${FUNCNAME[0]}(): }'

# Credit to Stack Overflow questioner Jiarro and answerer Dave Dopson
# http://stackoverflow.com/questions/59895/can-a-bash-script-tell-what-directory-its-stored-in
# http://stackoverflow.com/a/246128/424301
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
BASE_DIR="$DIR/.."
TOP_DIR="$BASE_DIR/.."
CREATIVE_ENGINE_DIR="$TOP_DIR/creative-engine"
export DIR BUILD_DIR TOP_DIR CREATIVE_ENGINE_DIR

######################### Main build ##################################

function finish {
    # Your cleanup code here
    if [[ -d "$CREATIVE_ENGINE_DIR" ]]; then
        cd "$BASE_DIR"
        rm -rf creative-engine
        ln -s "$CREATIVE_ENGINE_DIR" creative-engine
    fi
}
trap finish EXIT

cd "$BASE_DIR"
# Docker does not work with symlinks, so let's temporarily copy the
# the whole repo here instead. Fix it up in the exit trap. Ugh.
if [[ -L creative-engine ]]; then
    rm -f creative-engine
    cp -a "$CREATIVE_ENGINE_DIR" .
fi
#shellcheck disable=SC2086,SC2048
docker build . -t genus -f "$DIR/Dockerfile"

