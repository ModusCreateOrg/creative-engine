#!/usr/bin/env bash

if [ -z "$GENUS_ROOT" ]; then export GENUS_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}")/.." && pwd )"; fi
if [ -z "$ENGINE_ROOT" ]; then export ENGINE_ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}")/../creative-engine" && pwd )"; fi
