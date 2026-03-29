set -e
cmake -B build .
cmake --build build
#Disable ctest if making changes that could break testing
ctest --test-dir build
./build/Animation
convert -delay 3 -loop 0 output/frame*.png output/output.gif
