#!/bin/zsh
g++ -Wall -pthread -o .build/main.out main.cc &&
    .build/main.out 1>.build/image.ppm &&
    magick .build/image.ppm .build/image.png &&
    cp .build/image.png .build/"$(date '+%Y%m%d_%H%M%S')".png
