#!/bin/bash
# Usage: compare.sh <render_test_bin> <scene> <references_dir>
# Renders the scene and compares against the reference image.
# If no reference exists, saves the current output as the reference.

set -e

BINARY="$1"
SCENE="$2"
REFS_DIR="$3"

REFERENCE="$REFS_DIR/${SCENE}.png"
ACTUAL="/tmp/test_${SCENE}_$$.png"

# Run renderer
"$BINARY" "$SCENE" "$ACTUAL"

if [ ! -f "$REFERENCE" ]; then
    echo "No reference found for '$SCENE' — saving current output as reference."
    cp "$ACTUAL" "$REFERENCE"
    rm "$ACTUAL"
    exit 0
fi

# Compare pixel-for-pixel; 'compare' returns 1 if images differ
DIFF=$(compare -metric AE "$REFERENCE" "$ACTUAL" /dev/null 2>&1 || true)

rm "$ACTUAL"

if [ "$DIFF" = "0" ]; then
    echo "PASS: $SCENE"
    exit 0
else
    echo "FAIL: $SCENE — $DIFF pixels differ"
    exit 1
fi
