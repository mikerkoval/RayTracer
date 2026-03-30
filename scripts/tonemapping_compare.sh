#!/bin/bash
set -e
RENDER=./build/render
SCENE=scenes/tonemapping_test.json
TMP=$(mktemp /tmp/tonemapping_test_XXXX.json)

# None
sed 's/"tonemapping": ".*"/"tonemapping": "none"/' "$SCENE" \
  | sed 's/"gamma": .*/"gamma": 1.0/' > "$TMP"
echo "Rendering: none..."
$RENDER "$TMP" output/tonemapping_none.png

# ACES
sed 's/"tonemapping": ".*"/"tonemapping": "aces"/' "$SCENE" \
  | sed 's/"gamma": .*/"gamma": 1.0/' > "$TMP"
echo "Rendering: aces..."
$RENDER "$TMP" output/tonemapping_aces.png

rm "$TMP"
echo "Done. Compare output/tonemapping_none.png vs output/tonemapping_aces.png"
