#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "raylib.h"
#include <stdbool.h>

typedef enum {
    GAME_MAIN_MENU = 0,
    GAME_BLOCK_CRUSHER,
    //ill add more games here
} GameState;

typedef struct GameManager {
    GameState currentGame;
    bool isRunning;
    int screenWidth;
    int screenHeight;
} GameManager;

void InitGameManager(GameManager* manager);
void RunGameManager(GameManager* manager);
void DrawMainMenu(GameManager* manager);
void HandleMainMenuInput(GameManager* manager);
void CloseGameManager(GameManager* manager);

#endif