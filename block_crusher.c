#include "game_manager.h"
#include "block_crusher.h"
#include <stdlib.h>
#include <stdio.h>

#define BASE_WIDTH 800
#define BASE_HEIGHT 600
#define BALL_TRAIL_LENGTH 16

static BlockCrusherGame game;

static float resize(float value, int baseDimension, int currentDimension) {
    return value * (currentDimension / (float)baseDimension);
}

void InitGame(int screenWidth, int screenHeight) {
    game.ball.position = (Vector2){screenWidth/2, screenHeight/2};
    game.ball.speed = (Vector2){resize(5.0f, BASE_WIDTH, screenWidth), 
                               resize(5.0f, BASE_HEIGHT, screenHeight)};
    game.ball.radius = resize(8.0f, BASE_WIDTH, screenWidth);
    game.ball.active = true;
    game.ball.trailIndex = 0;
    
    for (int i = 0; i < BALL_TRAIL_LENGTH; i++) {
        game.ball.trail[i] = game.ball.position;
        game.ball.trailAlpha[i] = 0.0f;
        game.ball.trailSize[i] = game.ball.radius;
    }
    
    float pd_width = resize(120.0f, BASE_WIDTH, screenWidth);
    float pd_height = resize(12.0f, BASE_HEIGHT, screenHeight);
    game.paddle.rect = (Rectangle){screenWidth/2 - pd_width/2, 
                                  screenHeight - resize(40.0f, BASE_HEIGHT, screenHeight), 
                                  pd_width, pd_height};
    game.paddle.speed = (Vector2){resize(8.0f, BASE_WIDTH, screenWidth), 0.0f};
    game.paddle.active = true;
    
    int rows = 5;
    int cols = 10;
    game.blocksCount = rows * cols;
    
    if (game.blocks != NULL) {
        free(game.blocks);
    }
    
    game.blocks = (Block*)malloc(game.blocksCount * sizeof(Block));
    
    float blockWidth = resize(70.0f, BASE_WIDTH, screenWidth);
    float blockHeight = resize(20.0f, BASE_HEIGHT, screenHeight);
    float blockSX = resize(8.0f, BASE_WIDTH, screenWidth);
    float blockSY = resize(8.0f, BASE_HEIGHT, screenHeight);
    float blockTM = resize(70.0f, BASE_HEIGHT, screenHeight);
    
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int index = i * cols + j;
            game.blocks[index].rect = (Rectangle){
                j * (blockWidth + blockSX) + blockSX, 
                i * (blockHeight + blockSY) + blockTM, 
                blockWidth, 
                blockHeight
            };
            game.blocks[index].active = true;
        }
    }

    game.score = 0;
    game.lives = 3;
    game.gameOver = false;
    game.paused = false;
}

void UpdateGame(int screenWidth, int screenHeight, struct GameManager* manager) {
    if (IsKeyPressed(KEY_P)) game.paused = !game.paused;
    if (IsKeyPressed(KEY_R)) InitGame(screenWidth, screenHeight);
    if (IsKeyPressed(KEY_M)) {
        manager->currentGame = GAME_MAIN_MENU;
        return;
    }
    
    if (game.gameOver || game.paused) return;
    
    float baseSpeedX = resize(5.0f, BASE_WIDTH, screenWidth);
    float baseSpeedY = resize(5.0f, BASE_HEIGHT, screenHeight);
    float pd_speed = resize(8.0f, BASE_WIDTH, screenWidth);
    
    if (IsKeyDown(KEY_LEFT) && game.paddle.rect.x > 0) {
        game.paddle.rect.x -= pd_speed;
    }
    if (IsKeyDown(KEY_RIGHT) && game.paddle.rect.x < screenWidth - game.paddle.rect.width) {
        game.paddle.rect.x += pd_speed;
    }
    
    UpdateBT(&game.ball);
    
    if (game.ball.active) {
        game.ball.position.x += game.ball.speed.x;
        game.ball.position.y += game.ball.speed.y;
        
        if (game.ball.position.x >= screenWidth - game.ball.radius || game.ball.position.x <= game.ball.radius) {
            game.ball.speed.x *= -1;
        }
        if (game.ball.position.y <= game.ball.radius) {
            game.ball.speed.y *= -1;
        }
        
        if (game.ball.position.y >= screenHeight + game.ball.radius) {
            game.lives--;
            if (game.lives <= 0) {
                game.gameOver = true;
            } else {
                ResetBall(screenWidth, screenHeight);
            }
        }
        
        if (CheckCollisionCircleRec(game.ball.position, game.ball.radius, game.paddle.rect)) {
            game.ball.speed.y *= -1;
            
            float hitPosition = (game.ball.position.x - game.paddle.rect.x) / game.paddle.rect.width;
            game.ball.speed.x = (hitPosition - 0.5f) * baseSpeedX * 2;
        }
        
        for (int i = 0; i < game.blocksCount; i++) {
            if (game.blocks[i].active && CheckCollisionCircleRec(game.ball.position, game.ball.radius, game.blocks[i].rect)) {
                game.blocks[i].active = false;
                game.ball.speed.y *= -1;
                game.score += 10;
                
                bool all_gone = true;
                for (int j = 0; j < game.blocksCount; j++) {
                    if (game.blocks[j].active) {
                        all_gone = false;
                        break;
                    }
                }
                
                if (all_gone) {
                    game.ball.speed.x *= 1.1f;
                    game.ball.speed.y *= 1.1f;
                    InitGame(screenWidth, screenHeight);
                    game.score += 100;
                }
                
                break;
            }
        }
    }
}

void DrawBlockCrusher(int screenWidth, int screenHeight) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    for (int i = 0; i < game.blocksCount; i++) {
        if (game.blocks[i].active) {
            DrawRectangleRounded(game.blocks[i].rect, 0.3f, 8, WHITE);
            DrawRectangleRoundedLines(game.blocks[i].rect, 0.3f, 8, LIGHTGRAY);
        }
    }
    
    DrawRectangleRounded(game.paddle.rect, 0.5f, 8, LIGHTGRAY);
    
    if (game.ball.active) {
        DrawBallWithTrail(&game.ball);
    }
    
    int fontSize = resize(20, BASE_HEIGHT, screenHeight);
    int uiPadding = resize(10, BASE_HEIGHT, screenHeight);
    
    const char* scoreText = TextFormat("SCORE: %04d", game.score);
    int scoretxt_width = MeasureText(scoreText, fontSize);
    DrawText(scoreText, uiPadding, uiPadding, fontSize, GREEN);
    
    const char* livesText = TextFormat("LIVES: %d", game.lives);
    int livestxt_width = MeasureText(livesText, fontSize);
    DrawText(livesText, screenWidth - livestxt_width - uiPadding, uiPadding, fontSize, GREEN);
    
    if (game.paused) {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
        
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
    
    if (game.gameOver) {
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(BLACK, 0.7f));
        
        int go_fs = resize(50, BASE_HEIGHT, screenHeight);
        const char* gameOverText = "GAME OVER";
        int gotxt_width = MeasureText(gameOverText, go_fs);
        DrawText(gameOverText, screenWidth/2 - gotxt_width/2, 
                screenHeight/2 - resize(80, BASE_HEIGHT, screenHeight), go_fs, RED);
        
        int score_fs = resize(30, BASE_HEIGHT, screenHeight);
        const char* finalScoreText = TextFormat("FINAL SCORE: %04d", game.score);
        int finalscoretxt_width = MeasureText(finalScoreText, score_fs);
        DrawText(finalScoreText, screenWidth/2 - finalscoretxt_width/2, 
                screenHeight/2, score_fs, YELLOW);
        
        int rst_fs = resize(20, BASE_HEIGHT, screenHeight);
        const char* restartText = "Press R to restart";
        int rstxt_width = MeasureText(restartText, rst_fs);
        DrawText(restartText, screenWidth/2 - rstxt_width/2, 
                screenHeight/2 + resize(60, BASE_HEIGHT, screenHeight), rst_fs, WHITE);
    }
    
    int ctrl_fs = resize(18, BASE_HEIGHT, screenHeight);
    const char* controlsText = "CONTROLS: Arrow Keys = Move, P = Pause, R = Restart, M = Menu";
    int ctrltxt_width = MeasureText(controlsText, ctrl_fs);
    
    if (ctrltxt_width > screenWidth - uiPadding * 2) {
        ctrl_fs = resize(16, BASE_HEIGHT, screenHeight);
        ctrltxt_width = MeasureText(controlsText, ctrl_fs);
    }
    
    DrawText(controlsText, screenWidth/2 - ctrltxt_width/2, 
            screenHeight - ctrl_fs - uiPadding, ctrl_fs, GRAY);
    
    EndDrawing();
}

void CloseBlockCrusher(void) {
    if (game.blocks != NULL) {
        free(game.blocks);
        game.blocks = NULL;
    }
}

void ResetBall(int screenWidth, int screenHeight) {
    game.ball.position = (Vector2){screenWidth/2, screenHeight/2};
    game.ball.speed = (Vector2){resize(5.0f, BASE_WIDTH, screenWidth), 
                               resize(5.0f, BASE_HEIGHT, screenHeight)};
    game.ball.active = true;
    
    for (int i = 0; i < BALL_TRAIL_LENGTH; i++) {
        game.ball.trail[i] = game.ball.position;
        game.ball.trailAlpha[i] = 0.0f;
        game.ball.trailSize[i] = game.ball.radius;
    }
    game.ball.trailIndex = 0;
}

void UpdateBT(Ball *ball) {
    ball->trail[ball->trailIndex] = ball->position;
    ball->trailAlpha[ball->trailIndex] = 1.0f;
    ball->trailSize[ball->trailIndex] = ball->radius;
    ball->trailIndex = (ball->trailIndex + 1) % BALL_TRAIL_LENGTH;
    
    for (int i = 0; i < BALL_TRAIL_LENGTH; i++) {
        if (ball->trailAlpha[i] > 0.0f) {
            ball->trailAlpha[i] -= 1.0f / BALL_TRAIL_LENGTH;
            ball->trailSize[i] -= ball->radius / BALL_TRAIL_LENGTH;
            
            if (ball->trailAlpha[i] < 0.0f) ball->trailAlpha[i] = 0.0f;
            if (ball->trailSize[i] < 1.0f) ball->trailSize[i] = 1.0f;
        }
    }
}

void DrawBallWithTrail(Ball *ball) {
    for (int i = 0; i < BALL_TRAIL_LENGTH; i++) {
        int index = (ball->trailIndex + i) % BALL_TRAIL_LENGTH;
        
        if (ball->trailAlpha[index] > 0.0f) {
            float easedAlpha = ball->trailAlpha[index] * ball->trailAlpha[index];
            Color trailColor = Fade(WHITE, easedAlpha * 0.7f);
            DrawCircleV(ball->trail[index], ball->trailSize[index], trailColor);
            
            if (ball->trailAlpha[index] > 0.3f) {
                DrawCircleV(ball->trail[index], ball->trailSize[index] * 1.2f, 
                           Fade((Color){200, 200, 255, 255}, easedAlpha * 0.3f));
            }
        }
    }
    
    DrawCircleV(ball->position, ball->radius, WHITE);
    DrawCircleV(ball->position, ball->radius * 1.1f, Fade(WHITE, 0.3f));
    DrawCircle(ball->position.x - ball->radius/3, ball->position.y - ball->radius/3, 
               ball->radius/4, Fade(WHITE, 0.8f));
}