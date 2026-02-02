#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <stdbool.h>

// Game state enum - keep this here since it's used by game_manager
typedef enum {
    GAME_MAIN_MENU = 0,
    GAME_BLOCK_CRUSHER,
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