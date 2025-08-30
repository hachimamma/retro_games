#!/bin/bash

echo "Building Retro Games Collection..."

gcc -o retro_games main.c game_manager.c block_crusher.c -lraylib -lm -lpthread -ldl

if [ $? -eq 0 ]; then
    echo "Build successful! Starting game..."
    echo ""
    echo "CONTROLS:"
    echo "  Main Menu: Press 1 for Block Crusher, 4 to Exit"
    echo "  In Game: Arrow Keys = Move, P = Pause, R = Restart, M = Return to Menu"
    echo ""
    ./retro_games
else
    echo "Build failed! Pls inform the developer, or try to fix urself!"
    exit 1
fi