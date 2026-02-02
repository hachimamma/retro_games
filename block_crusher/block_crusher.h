#ifndef BLOCK_CRUSHER_H
#define BLOCK_CRUSHER_H

#include "raylib.h"
#include <stdbool.h>

#define BASE_WIDTH 800
#define BASE_HEIGHT 600
#define BALL_TRAIL_LENGTH 16

#define BASE_BALL_SPEED 5.0f
#define BASE_PADDLE_SPEED 8.0f
#define BALL_RADIUS 8.0f
#define PADDLE_WIDTH 120.0f
#define PADDLE_HEIGHT 12.0f
#define PADDLE_Y_OFFSET 40.0f
#define BLOCK_WIDTH 70.0f
#define BLOCK_HEIGHT 20.0f
#define BLOCK_MARGIN 8.0f
#define BLOCK_TOP_MARGIN 70.0f
#define POWERUP_SPEED 3.0f
#define POWERUP_RADIUS 8.0f
#define LEVEL_SPEED_INCREASE 1.1f
#define LEVEL_SCORE_BONUS 100

// Forward declarations
typedef struct GameManager GameManager;

typedef struct Block {
    Rectangle rect;
    bool active;
    Color color;
    int hitsRequired;
    int hitsTaken;
} Block;

typedef struct Paddle {
    Rectangle rect;
    Vector2 speed;
    bool active;
    float baseWidth;
} Paddle;

typedef struct Ball {
    Vector2 position;
    Vector2 speed;
    bool active;
    float radius;
    Vector2 trail[BALL_TRAIL_LENGTH];
    float trailAlpha[BALL_TRAIL_LENGTH];
    float trailSize[BALL_TRAIL_LENGTH];
    int trailIndex;
} Ball;

typedef struct BlockCrusherGame {
    Paddle paddle;
    Block* blocks;
    int blocksCount;
    int score;
    int lives;
    bool gameOver;
    bool paused;
} BlockCrusherGame;

// Utility function
static inline float resize(float value, int baseDimension, int currentDimension) {
    return value * (currentDimension / (float)baseDimension);
}

// Public API functions
void InitBlockCrusher(int screenWidth, int screenHeight);
void UpdateBlockCrusher(int screenWidth, int screenHeight, GameManager* manager);
void DrawBlockCrusher(int screenWidth, int screenHeight);
void CloseBlockCrusher(void);

#endif