#!/bin/bash

if [ "$1" == "debug" ]
then
    FLAGS="-DDEBUG"
else
    FLAGS="-O3"
fi

gcc *.c -o game $FLAGS $(pkg-config --cflags --libs allegro-5 allegro_primitives-5 allegro_main-5 allegro_audio-5 allegro_acodec-5 allegro_image-5 allegro_font-5)
