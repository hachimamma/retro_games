#include "game_manager.h"
#include "block_crusher.h"

#define BASE_WIDTH 800
#define BASE_HEIGHT 600

void InitGameManager(GameManager* manager) {
    manager->currentGame = GAME_MAIN_MENU;
    manager->isRunning = true;
    manager->screenWidth = BASE_WIDTH;
    manager->screenHeight = BASE_HEIGHT;
    
    SetTraceLogLevel(LOG_NONE);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(manager->screenWidth, manager->screenHeight, "Retro Games Collection");
    SetTargetFPS(60);
}

void RunGameManager(GameManager* manager) {
    while (!WindowShouldClose() && manager->isRunning) {
        if (IsWindowResized()) {
            manager->screenWidth = GetScreenWidth();
            manager->screenHeight = GetScreenHeight();
            
            switch (manager->currentGame) {
                case GAME_BLOCK_CRUSHER:
                    InitBlockCrusher(manager->screenWidth, manager->screenHeight);
                    break;
                default:
                    break;
            }
        }
        
        switch (manager->currentGame) {
            case GAME_BLOCK_CRUSHER:
                UpdateBlockCrusher(manager->screenWidth, manager->screenHeight, manager);
                DrawBlockCrusher(manager->screenWidth, manager->screenHeight);
                break;
            default:
                DrawMainMenu(manager);
                HandleMainMenuInput(manager);
                break;
        }
    }
}

void DrawMainMenu(GameManager* manager) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    int tt_s = 60;
    const char* title = "RETRO GAMES COLLECTION";
    int titleWidth = MeasureText(title, tt_s);
    DrawText(title, manager->screenWidth/2 - titleWidth/2, 100, tt_s, YELLOW);
    
    int optionSize = 30;
    const char* options[] = {
        "1. Block Crusher",
        "2. Snake (Coming Soon)",
        "3. Tetris (Coming Soon)",
        "4. Exit"
    };
    
    for (int i = 0; i < 4; i++) {
        int optionWidth = MeasureText(options[i], optionSize);
        DrawText(options[i], manager->screenWidth/2 - optionWidth/2,
                250 + i * 50, optionSize, WHITE);
    }
    
    int inst_s = 20;
    const char* instruction = "Press number key to select a game";
    int instructionWidth = MeasureText(instruction, inst_s);
    DrawText(instruction, manager->screenWidth/2 - instructionWidth/2,
            manager->screenHeight - 50, inst_s, GRAY);
    
    EndDrawing();
}

void HandleMainMenuInput(GameManager* manager) {
    if (IsKeyPressed(KEY_ONE)) {
        manager->currentGame = GAME_BLOCK_CRUSHER;
        InitBlockCrusher(manager->screenWidth, manager->screenHeight);
    }
    else if (IsKeyPressed(KEY_FOUR)) {
        manager->isRunning = false;
    }
}

void CloseGameManager(GameManager* manager) {
    switch (manager->currentGame) {
        case GAME_BLOCK_CRUSHER:
            CloseBlockCrusher();
            break;
        default:
            break;
    }
    CloseWindow();
}