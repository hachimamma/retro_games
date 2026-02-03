#include "block_crusher_rendering.h"
#include "block_crusher_balls.h"
#include "block_crusher_levels.h"
#include "block_crusher_powerups.h"
#include <math.h>

void DrawBlocks(BlockCrusherGame* game) {
    for (int i = 0; i < game->blocksCount; i++) {
        if (!game->blocks[i].active) continue;
        
        float healthRatio = 1.0f - (game->blocks[i].hitsTaken / (float)game->blocks[i].hitsRequired);
        Color blockColor = ColorAlpha(game->blocks[i].color, healthRatio * 0.8f + 0.2f);
        
        DrawRectangleRounded(game->blocks[i].rect, 0.2f, 6, blockColor);
        DrawRectangleRoundedLines(game->blocks[i].rect, 0.2f, 6, ColorAlpha(blockColor, 1.0f));
        
        if (game->blocks[i].hitsRequired > 1) {
            const char* hitText = TextFormat("%d", game->blocks[i].hitsRequired - game->blocks[i].hitsTaken);
            int textSize = game->blocks[i].rect.height * 0.6f;
            int textWidth = MeasureText(hitText, textSize);
            DrawText(hitText, 
                     game->blocks[i].rect.x + game->blocks[i].rect.width/2 - textWidth/2,
                     game->blocks[i].rect.y + game->blocks[i].rect.height/2 - textSize/2,
                     textSize, BLACK);
        }
    }
}

void DrawPaddle(Paddle* paddle) {
    Color paddleColor = LIGHTGRAY;
    if (paddleEnlargeTimer > 0) {
        float pulse = sinf(GetTime() * 10.0f) * 0.1f + 0.9f;
        paddleColor = ColorAlpha(GREEN, pulse);
    }
    DrawRectangleRounded(paddle->rect, 0.5f, 8, paddleColor);
}

void DrawUI(BlockCrusherGame* game, int screenWidth, int screenHeight) {
    int fontSize = resize(20, BASE_HEIGHT, screenHeight);
    int uiPadding = resize(10, BASE_HEIGHT, screenHeight);
    
    const char* scoreText = TextFormat("SCORE: %06d", game->score);
    DrawText(scoreText, uiPadding, uiPadding, fontSize, GREEN);
    
    const char* livesText = TextFormat("LIVES: %d", game->lives);
    int livesWidth = MeasureText(livesText, fontSize);
    DrawText(livesText, screenWidth - livesWidth - uiPadding, uiPadding, fontSize, GREEN);
    
    const char* ballsText = TextFormat("BALLS: %d", activeBallCount);
    int ballsWidth = MeasureText(ballsText, fontSize);
    DrawText(ballsText, screenWidth/2 - ballsWidth/2, uiPadding, fontSize, YELLOW);
    
    const char* levelText = TextFormat("LEVEL: %d/%d", currentLevel + 1, totalLevels);
    int levelWidth = MeasureText(levelText, fontSize);
    DrawText(levelText, screenWidth/2 - levelWidth/2, uiPadding + fontSize + 5, fontSize, ORANGE);
    
    if (paddleEnlargeTimer > 0) {
        const char* paddleTimer = TextFormat("PADDLE: %.1fs", paddleEnlargeTimer);
        DrawText(paddleTimer, uiPadding, uiPadding + fontSize + 5, fontSize, GREEN);
    }
    
    if (slowBallTimer > 0) {
        const char* slowTimer = TextFormat("SLOW: %.1fs", slowBallTimer);
        int slowWidth = MeasureText(slowTimer, fontSize);
        DrawText(slowTimer, screenWidth - slowWidth - uiPadding, uiPadding + fontSize + 5, fontSize, YELLOW);
    }
    
    if (game->paused) {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.85f));
        
        int pause_fs = resize(40, BASE_HEIGHT, screenHeight);
        const char* pauseText = "GAME PAUSED";
        int pausetxt_width = MeasureText(pauseText, pause_fs);
        DrawText(pauseText, screenWidth/2 - pausetxt_width/2, 
                screenHeight/2 - resize(60, BASE_HEIGHT, screenHeight), pause_fs, YELLOW);
        
        int ins_fs = resize(20, BASE_HEIGHT, screenHeight);
        const char* instructionText = "Press P to continue";
        int instxt_width = MeasureText(instructionText, ins_fs);
        DrawText(instructionText, screenWidth/2 - instxt_width/2, 
                screenHeight/2 + resize(20, BASE_HEIGHT, screenHeight), ins_fs, WHITE);
    }
    
    if (game->gameOver) {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.85f));
        
        int go_fs = resize(50, BASE_HEIGHT, screenHeight);
        const char* gameOverText = "GAME OVER";
        int gotxt_width = MeasureText(gameOverText, go_fs);
        DrawText(gameOverText, screenWidth/2 - gotxt_width/2, 
                screenHeight/2 - resize(80, BASE_HEIGHT, screenHeight), go_fs, RED);
        
        int score_fs = resize(30, BASE_HEIGHT, screenHeight);
        const char* finalScoreText = TextFormat("FINAL SCORE: %06d", game->score);
        int finalscoretxt_width = MeasureText(finalScoreText, score_fs);
        DrawText(finalScoreText, screenWidth/2 - finalscoretxt_width/2, 
                screenHeight/2, score_fs, YELLOW);
        
        const char* levelReachedText = TextFormat("LEVEL REACHED: %d", currentLevel + 1);
        int levelReachedWidth = MeasureText(levelReachedText, score_fs);
        DrawText(levelReachedText, screenWidth/2 - levelReachedWidth/2, 
                screenHeight/2 + 40, score_fs, ORANGE);
        
        int rst_fs = resize(20, BASE_HEIGHT, screenHeight);
        const char* restartText = "Press R to restart";
        int rstxt_width = MeasureText(restartText, rst_fs);
        DrawText(restartText, screenWidth/2 - rstxt_width/2, 
                screenHeight/2 + resize(100, BASE_HEIGHT, screenHeight), rst_fs, WHITE);
    }
    
    int ctrl_fs = resize(14, BASE_HEIGHT, screenHeight);
    const char* controlsText = "CONTROLS: Arrow Keys = Move, P = Pause, R = Restart, M = Menu";
    int ctrltxt_width = MeasureText(controlsText, ctrl_fs);
    DrawText(controlsText, screenWidth/2 - ctrltxt_width/2, 
            screenHeight - ctrl_fs - uiPadding, ctrl_fs, GRAY);
    
    if (paddleEnlargeTimer > 0 || slowBallTimer > 0) {
        int legend_fs = resize(12, BASE_HEIGHT, screenHeight);
        const char* legendText = "POWER-UPS: Green = Big Paddle, Yellow = Slow Ball, Blue = 2x Balls, Red = +1 Life";
        int legendWidth = MeasureText(legendText, legend_fs);
        
        if (legendWidth > screenWidth - uiPadding * 2) {
            legend_fs = resize(10, BASE_HEIGHT, screenHeight);
            legendWidth = MeasureText(legendText, legend_fs);
        }
        
        DrawText(legendText, screenWidth/2 - legendWidth/2, 
                screenHeight - ctrl_fs - legend_fs - uiPadding - 5, legend_fs, DARKGRAY);
    }
}