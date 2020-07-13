#!/bin/zsh
g++ -Wall -o /home/sean/cstar/raytracer/.build/main.out main.cc &&
    /home/sean/cstar/raytracer/.build/main.out 1>.build/image.ppm &&
    magick .build/image.ppm .build/image.png
