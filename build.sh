#!/bin/bash

gcc main.c -o game $(pkg-config --cflags --libs allegro-5 allegro_primitives-5 allegro_main-5 allegro_image-5)
