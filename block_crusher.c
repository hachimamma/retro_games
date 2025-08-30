#include "game_manager.h"
#include "block_crusher.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define BASE_WIDTH 800
#define BASE_HEIGHT 600
#define BALL_TRAIL_LENGTH 16
#define MAX_BALLS 16
#define MAX_POWERUPS 10
#define POWERUP_DROP_CHANCE 30

static BlockCrusherGame game;

typedef struct {
    Vector2 position;
    Vector2 speed;
    bool active;
    float radius;
} PowerUp;

static PowerUp powerups[MAX_POWERUPS];
static Ball balls[MAX_BALLS];
static int activeBallCount = 1;

static float resize(float value, int baseDimension, int currentDimension) {
    return value * (currentDimension / (float)baseDimension);
}

void InitBlockCrusher(int screenWidth, int screenHeight) {
    balls[0].position = (Vector2){screenWidth/2, screenHeight/2};
    balls[0].speed = (Vector2){resize(5.0f, BASE_WIDTH, screenWidth), 
                               resize(5.0f, BASE_HEIGHT, screenHeight)};
    balls[0].radius = resize(8.0f, BASE_WIDTH, screenWidth);
    balls[0].active = true;
    balls[0].trailIndex = 0;
    
    for (int i = 0; i < BALL_TRAIL_LENGTH; i++) {
        balls[0].trail[i] = balls[0].position;
        balls[0].trailAlpha[i] = 0.0f;
        balls[0].trailSize[i] = balls[0].radius;
    }
    
    for (int i = 1; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }
    activeBallCount = 1;
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerups[i].active = false;
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
    
    srand(time(NULL));
}

void SpawnPowerUp(float x, float y, int screenWidth, int screenHeight) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) {
            powerups[i].position = (Vector2){x, y};
            powerups[i].speed = (Vector2){0, resize(3.0f, BASE_HEIGHT, screenHeight)};
            powerups[i].radius = resize(8.0f, BASE_WIDTH, screenWidth);
            powerups[i].active = true;
            break;
        }
    }
}

void DoubleBalls(int screenWidth, int screenHeight) {
    int targetBallCount = activeBallCount * 2;
    if (targetBallCount > MAX_BALLS) targetBallCount = MAX_BALLS;
    
    Ball activeBalls[MAX_BALLS];
    int currentActiveBalls = 0;
    
    for (int i = 0; i < MAX_BALLS; i++) {
        if (balls[i].active) {
            activeBalls[currentActiveBalls] = balls[i];
            currentActiveBalls++;
        }
    }
    
    activeBallCount = 0;
    for (int i = 0; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }
    
    int ballsToAdd = (targetBallCount < currentActiveBalls * 2) ? targetBallCount : currentActiveBalls * 2;
    
    for (int i = 0; i < ballsToAdd && i < MAX_BALLS; i++) {
        int sourceIndex = i % currentActiveBalls;
        balls[i] = activeBalls[sourceIndex];
        
        if (i >= currentActiveBalls) {
            float angle = (rand() % 120 - 60) * (3.14159f / 180.0f);
            float cos_a = cosf(angle);
            float sin_a = sinf(angle);
            
            float newSpeedX = balls[i].speed.x * cos_a - balls[i].speed.y * sin_a;
            float newSpeedY = balls[i].speed.x * sin_a + balls[i].speed.y * cos_a;
            
            balls[i].speed.x = newSpeedX;
            balls[i].speed.y = newSpeedY;
        }
        
        balls[i].trailIndex = 0;
        for (int k = 0; k < BALL_TRAIL_LENGTH; k++) {
            balls[i].trail[k] = balls[i].position;
            balls[i].trailAlpha[k] = 0.0f;
            balls[i].trailSize[k] = balls[i].radius;
        }
        
        balls[i].active = true;
        activeBallCount++;
    }
}

void UpdateBlockCrusher(int screenWidth, int screenHeight, struct GameManager* manager) {
    if (IsKeyPressed(KEY_P)) game.paused = !game.paused;
    if (IsKeyPressed(KEY_R)) InitBlockCrusher(screenWidth, screenHeight);
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
    
    for (int ballIdx = 0; ballIdx < MAX_BALLS; ballIdx++) {
        if (!balls[ballIdx].active) continue;
        
        UpdateBT(&balls[ballIdx]);
        
        balls[ballIdx].position.x += balls[ballIdx].speed.x;
        balls[ballIdx].position.y += balls[ballIdx].speed.y;
        
        if (balls[ballIdx].position.x >= screenWidth - balls[ballIdx].radius || 
            balls[ballIdx].position.x <= balls[ballIdx].radius) {
            balls[ballIdx].speed.x *= -1;
        }
        if (balls[ballIdx].position.y <= balls[ballIdx].radius) {
            balls[ballIdx].speed.y *= -1;
        }
        
        if (balls[ballIdx].position.y >= screenHeight + balls[ballIdx].radius) {
            balls[ballIdx].active = false;
            activeBallCount--;
            
            if (activeBallCount <= 0) {
                game.lives--;
                if (game.lives <= 0) {
                    game.gameOver = true;
                } else {
                    ResetBall(screenWidth, screenHeight);
                }
            }
        }
        
        if (CheckCollisionCircleRec(balls[ballIdx].position, balls[ballIdx].radius, game.paddle.rect)) {
            balls[ballIdx].speed.y *= -1;
            
            float hitPosition = (balls[ballIdx].position.x - game.paddle.rect.x) / game.paddle.rect.width;
            balls[ballIdx].speed.x = (hitPosition - 0.5f) * baseSpeedX * 2;
        }
        
        for (int i = 0; i < game.blocksCount; i++) {
            if (game.blocks[i].active && 
                CheckCollisionCircleRec(balls[ballIdx].position, balls[ballIdx].radius, game.blocks[i].rect)) {
                
                game.blocks[i].active = false;
                balls[ballIdx].speed.y *= -1;
                game.score += 10;
                
                if (rand() % 100 < POWERUP_DROP_CHANCE) {
                    float centerX = game.blocks[i].rect.x + game.blocks[i].rect.width / 2;
                    float centerY = game.blocks[i].rect.y + game.blocks[i].rect.height / 2;
                    SpawnPowerUp(centerX, centerY, screenWidth, screenHeight);
                }
                
                bool all_gone = true;
                for (int j = 0; j < game.blocksCount; j++) {
                    if (game.blocks[j].active) {
                        all_gone = false;
                        break;
                    }
                }
                
                if (all_gone) {
                    for (int k = 0; k < MAX_BALLS; k++) {
                        if (balls[k].active) {
                            balls[k].speed.x *= 1.1f;
                            balls[k].speed.y *= 1.1f;
                        }
                    }
                    InitBlockCrusher(screenWidth, screenHeight);
                    game.score += 100;
                }
                
                break;
            }
        }
    }
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) continue;
        
        powerups[i].position.y += powerups[i].speed.y;
        
        if (CheckCollisionCircleRec(powerups[i].position, powerups[i].radius, game.paddle.rect)) {
            powerups[i].active = false;
            DoubleBalls(screenWidth, screenHeight);
            game.score += 50;
        }
        
        if (powerups[i].position.y > screenHeight + powerups[i].radius) {
            powerups[i].active = false;
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
    
    for (int i = 0; i < MAX_BALLS; i++) {
        if (balls[i].active) {
            DrawBallWithTrail(&balls[i]);
        }
    }
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (powerups[i].active) {
            DrawCircleV(powerups[i].position, powerups[i].radius, BLUE);
            DrawCircleV(powerups[i].position, powerups[i].radius * 1.2f, Fade(SKYBLUE, 0.5f));
            DrawCircle(powerups[i].position.x - powerups[i].radius/3, 
                      powerups[i].position.y - powerups[i].radius/3, 
                      powerups[i].radius/4, Fade(WHITE, 0.8f));
        }
    }
    
    int fontSize = resize(20, BASE_HEIGHT, screenHeight);
    int uiPadding = resize(10, BASE_HEIGHT, screenHeight);
    
    const char* scoreText = TextFormat("SCORE: %04d", game.score);
    int scoretxt_width = MeasureText(scoreText, fontSize);
    DrawText(scoreText, uiPadding, uiPadding, fontSize, GREEN);
    
    const char* livesText = TextFormat("LIVES: %d", game.lives);
    int livestxt_width = MeasureText(livesText, fontSize);
    DrawText(livesText, screenWidth - livestxt_width - uiPadding, uiPadding, fontSize, GREEN);
    
    const char* ballsText = TextFormat("BALLS: %d", activeBallCount);
    int ballstxt_width = MeasureText(ballsText, fontSize);
    DrawText(ballsText, screenWidth/2 - ballstxt_width/2, uiPadding, fontSize, YELLOW);
    
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
    
    int ctrl_fs = resize(16, BASE_HEIGHT, screenHeight);
    const char* controlsText = "CONTROLS: Arrow Keys = Move, P = Pause, R = Restart, M = Menu | Blue balls = Double your balls!";
    int ctrltxt_width = MeasureText(controlsText, ctrl_fs);
    
    if (ctrltxt_width > screenWidth - uiPadding * 2) {
        ctrl_fs = resize(14, BASE_HEIGHT, screenHeight);
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
    balls[0].position = (Vector2){screenWidth/2, screenHeight/2};
    balls[0].speed = (Vector2){resize(5.0f, BASE_WIDTH, screenWidth), 
                               resize(5.0f, BASE_HEIGHT, screenHeight)};
    balls[0].active = true;
    
    for (int i = 0; i < BALL_TRAIL_LENGTH; i++) {
        balls[0].trail[i] = balls[0].position;
        balls[0].trailAlpha[i] = 0.0f;
        balls[0].trailSize[i] = balls[0].radius;
    }
    balls[0].trailIndex = 0;
    
    for (int i = 1; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }
    activeBallCount = 1;
    
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerups[i].active = false;
    }
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