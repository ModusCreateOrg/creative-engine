#!/usr/bin/env bash

MY_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")" && pwd )"

source "$MY_DIR/../env.sh"

(cd $GENUS_ROOT && git checkout master)
(cd $GENUS_ROOT && git pull upstream master)

(cd $ENGINE_ROOT && git checkout master)
(cd $ENGINE_ROOT && git pull upstream master)

