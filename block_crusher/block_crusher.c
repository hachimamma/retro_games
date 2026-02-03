#include "block_crusher.h"
#include "block_crusher_balls.h"
#include "block_crusher_powerups.h"
#include "block_crusher_levels.h"
#include "block_crusher_rendering.h"
#include "../game_manager.h"
#include <stdlib.h>
#include <time.h>

static BlockCrusherGame game = {0};

static void UpdatePaddle(Paddle* paddle, int screenWidth) {
    if (!paddle->active) return;
    
    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
        paddle->rect.x -= paddle->speed.x;
    }
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
        paddle->rect.x += paddle->speed.x;
    }
    
    if (paddle->rect.x < 0) {
        paddle->rect.x = 0;
    }
    if (paddle->rect.x + paddle->rect.width > screenWidth) {
        paddle->rect.x = screenWidth - paddle->rect.width;
    }
}

void InitBlockCrusher(int screenWidth, int screenHeight) {
    srand(time(NULL));
    
    game.score = 0;
    game.lives = 3;
    game.gameOver = false;
    game.paused = false;
    game.consecutiveHits = 0;
    
    InitBalls(screenWidth, screenHeight);
    InitPowerUps();
    SetCurrentLevel(0);
    InitPaddle(&game.paddle, screenWidth, screenHeight);
    CreateBlocksForLevel(&game, screenWidth, screenHeight);
}

void UpdateBlockCrusher(int screenWidth, int screenHeight, GameManager* manager) {
    if (IsKeyPressed(KEY_P)) game.paused = !game.paused;
    if (IsKeyPressed(KEY_R)) InitBlockCrusher(screenWidth, screenHeight);
    if (IsKeyPressed(KEY_M)) {
        manager->currentGame = GAME_MAIN_MENU;
        return;
    }
    
    if (game.gameOver || game.paused) return;
    
    UpdatePaddle(&game.paddle, screenWidth);
    UpdateBalls(&game, screenWidth, screenHeight);
    UpdatePowerUps(&game, screenWidth, screenHeight);
    UpdatePowerUpTimers(&game.paddle);
    CheckLevelCompletion(&game, screenWidth, screenHeight);
}

void DrawBlockCrusher(int screenWidth, int screenHeight) {
    BeginDrawing();
    ClearBackground((Color){10, 10, 20, 255});
    
    DrawBlocks(&game);
    DrawPaddle(&game.paddle);
    DrawBalls();
    DrawPowerUps();
    DrawUI(&game, screenWidth, screenHeight);
    
    EndDrawing();
}

void CloseBlockCrusher(void) {
    if (game.blocks != NULL) {
        free(game.blocks);
        game.blocks = NULL;
    }
    ClosePowerUps();
}