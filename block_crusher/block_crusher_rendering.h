#ifndef BLOCK_CRUSHER_RENDERING_H
#define BLOCK_CRUSHER_RENDERING_H

#include "block_crusher.h"

void DrawBlocks(BlockCrusherGame* game);
void DrawPaddle(Paddle* paddle);
void DrawUI(BlockCrusherGame* game, int screenWidth, int screenHeight);

#endif