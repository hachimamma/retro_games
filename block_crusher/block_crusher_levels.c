#include "block_crusher_levels.h"
#include <stdlib.h>

int currentLevel = 0;
int totalLevels = 5;

static Level levels[] = {
    {5, 10, WHITE, 0},
    {6, 12, BLUE, 500},
    {7, 14, RED, 1000},
    {8, 16, GREEN, 2000},
    {9, 18, YELLOW, 4000},
};

void SetCurrentLevel(int level) {
    currentLevel = level;
}

void InitPaddle(Paddle* paddle, int screenWidth, int screenHeight) {
    float pd_width = resize(PADDLE_WIDTH, BASE_WIDTH, screenWidth);
    float pd_height = resize(PADDLE_HEIGHT, BASE_HEIGHT, screenHeight);
    paddle->rect = (Rectangle){
        screenWidth/2 - pd_width/2, 
        screenHeight - resize(PADDLE_Y_OFFSET, BASE_HEIGHT, screenHeight), 
        pd_width, 
        pd_height
    };
    paddle->speed = (Vector2){resize(BASE_PADDLE_SPEED, BASE_WIDTH, screenWidth), 0.0f};
    paddle->active = true;
    paddle->baseWidth = pd_width;
}

void CreateBlocksForLevel(BlockCrusherGame* game, int screenWidth, int screenHeight) {
    Level* level = &levels[currentLevel];
    game->blocksCount = level->rows * level->cols;
    
    if (game->blocks != NULL) {
        free(game->blocks);
    }
    
    game->blocks = (Block*)malloc(game->blocksCount * sizeof(Block));
    if (game->blocks == NULL) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for blocks!");
        game->blocksCount = 0;
        return;
    }
    
    float blockWidth = resize(BLOCK_WIDTH, BASE_WIDTH, screenWidth);
    float blockHeight = resize(BLOCK_HEIGHT, BASE_HEIGHT, screenHeight);
    float blockSX = resize(BLOCK_MARGIN, BASE_WIDTH, screenWidth);
    float blockSY = resize(BLOCK_MARGIN, BASE_HEIGHT, screenHeight);
    float blockTM = resize(BLOCK_TOP_MARGIN, BASE_HEIGHT, screenHeight);
    
    for (int i = 0; i < level->rows; i++) {
        for (int j = 0; j < level->cols; j++) {
            int index = i * level->cols + j;
            game->blocks[index].rect = (Rectangle){
                j * (blockWidth + blockSX) + blockSX, 
                i * (blockHeight + blockSY) + blockTM, 
                blockWidth, 
                blockHeight
            };
            game->blocks[index].active = true;
            game->blocks[index].color = level->blockColor;
            game->blocks[index].hitsRequired = (currentLevel / 2) + 1;
            game->blocks[index].hitsTaken = 0;
        }
    }
}

void CheckLevelCompletion(BlockCrusherGame* game, int screenWidth, int screenHeight) {
    bool allBlocksGone = true;
    for (int i = 0; i < game->blocksCount; i++) {
        if (game->blocks[i].active) {
            allBlocksGone = false;
            break;
        }
    }
    
    if (allBlocksGone) {
        currentLevel++;
        if (currentLevel >= totalLevels) {
            currentLevel = totalLevels - 1;
            game->score += LEVEL_SCORE_BONUS * 2;
        } else {
            game->score += LEVEL_SCORE_BONUS;
        }
        
        CreateBlocksForLevel(game, screenWidth, screenHeight);
    }
}