#ifndef BLOCK_CRUSHER_BALLS_H
#define BLOCK_CRUSHER_BALLS_H

#include "block_crusher.h"

#define MAX_BALLS 32

// Extern declarations
extern Ball balls[MAX_BALLS];
extern int activeBallCount;

void InitBalls(int screenWidth, int screenHeight);
void DoubleBalls(void);
void ResetBall(int screenWidth, int screenHeight);
void UpdateBalls(BlockCrusherGame* game, int screenWidth, int screenHeight);
void UpdateBallTrail(Ball *ball);
void DrawBallWithTrail(Ball *ball);
void DrawBalls(void);

#endif