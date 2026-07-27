#!/usr/bin/env bash

export LC_ALL=C
set -eu

uname_S=$(uname -s 2>/dev/null || echo not)

if [ "$uname_S" = "Darwin" ]; then
    PARAMS_DIR="$HOME/Library/Application Support/SpectreParams"
else
    PARAMS_DIR="$HOME/.spectre-params"
fi

cat <<EOF

Download Spectre proving parameters if needed for legacy Sprout support.

Parameters for Sapling, Orchard, and other pools are bundled with spectred or
generated at runtime. Sprout parameters may still be downloaded separately.

Place Sprout parameter files at:
$PARAMS_DIR/sprout-groth16.params

EOF

exit 0
