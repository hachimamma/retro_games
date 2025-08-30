#ifndef BLOCK_CRUSHER_H
#define BLOCK_CRUSHER_H

#include "raylib.h"
#include <stdbool.h>

typedef struct {
    Vector2 position;
    Vector2 speed;
    float radius;
    bool active;
    Vector2 trail[16];
    float trailAlpha[16];
    float trailSize[16];
    int trailIndex;
} Ball;

typedef struct {
    Rectangle rect;
    Vector2 speed;
    bool active;
} Paddle;

typedef struct {
    Rectangle rect;
    bool active;
} Block;

typedef struct {
    Ball ball;
    Paddle paddle;
    Block* blocks;
    int blocksCount;
    int score;
    int lives;
    bool gameOver;
    bool paused;
} BlockCrusherGame;

struct GameManager;

void InitBlockCrusher(int screenWidth, int screenHeight);
void UpdateBlockCrusher(int screenWidth, int screenHeight, struct GameManager* manager);
void DrawBlockCrusher(int screenWidth, int screenHeight);
void CloseBlockCrusher(void);
void ResetBall(int screenWidth, int screenHeight);
void UpdateBT(Ball *ball);
void DrawBallWithTrail(Ball *ball);

#endif