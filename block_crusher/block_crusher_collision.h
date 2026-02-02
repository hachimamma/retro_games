#ifndef BLOCK_CRUSHER_COLLISION_H
#define BLOCK_CRUSHER_COLLISION_H

#include "block_crusher.h"
#include "block_crusher_balls.h"

void HandleBallBlockCollision(int ballIdx, int blockIdx, BlockCrusherGame* game);
void HandleBallPaddleCollision(int ballIdx, Paddle* paddle);
void UpdatePaddle(Paddle* paddle, int screenWidth);

#endif