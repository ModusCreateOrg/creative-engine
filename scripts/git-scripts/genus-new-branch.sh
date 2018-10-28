#!/usr/bin/env bash

MY_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}")" && pwd )"

source "$MY_DIR/../env.sh"

if [ "$#" -ne 1 ]; then
    echo "The branchname is missing!"
    echo "Usage: genus-new-branch.sh <branchname>"
    exit
fi

(cd $GENUS_ROOT && git checkout master)
(cd $GENUS_ROOT && git pull upstream master)
(cd $GENUS_ROOT && git checkout -b $1)
