#ifndef BLOCK_CRUSHER_COLLISION_H
#define BLOCK_CRUSHER_COLLISION_H

#include "block_crusher.h"

void HandleBallPaddleCollision(int ballIndex, Paddle* paddle);
void HandleBallBlockCollision(int ballIndex, int blockIndex, BlockCrusherGame* game);

#endif