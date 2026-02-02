#!/bin/bash

echo "Building Retro Games Collection..."

gcc -o retro_games \
    main.c \
    game_manager.c \
    block_crusher/block_crusher.c \
    block_crusher/block_crusher_balls.c \
    block_crusher/block_crusher_powerups.c \
    block_crusher/block_crusher_levels.c \
    block_crusher/block_crusher_collision.c \
    block_crusher/block_crusher_rendering.c \
    -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

if [ $? -eq 0 ]; then
    echo "Build successful! Running game..."
    ./retro_games
else
    echo "Build failed! Check your code for errors."
fi