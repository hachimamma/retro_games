#ifndef BLOCK_CRUSHER_LEVELS_H
#define BLOCK_CRUSHER_LEVELS_H

#include "block_crusher.h"

typedef struct {
    int rows;
    int cols;
    Color blockColor;
    int requiredScore;
} Level;

// Extern declarations
extern int currentLevel;
extern int totalLevels;

void CreateBlocksForLevel(BlockCrusherGame* game, int screenWidth, int screenHeight);
void CheckLevelCompletion(BlockCrusherGame* game, int screenWidth, int screenHeight);
void SetCurrentLevel(int level);
void InitPaddle(Paddle* paddle, int screenWidth, int screenHeight);

#endif