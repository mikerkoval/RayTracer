cmake -B build .
cmake --build build
./build/Animation
convert -delay 3 -loop 0 output/frame*.png output.gif
