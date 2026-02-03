#include "block_crusher_levels.h"
#include <stdlib.h>
#include <math.h>

int currentLevel = 0;
int totalLevels = 10;

static Level levels[] = {
    // Level 1 - Simple rows
    {4, 8, SKYBLUE, 0},
    
    // Level 2 - Pyramid start
    {5, 9, BLUE, 400},
    
    // Level 3 - Checkerboard
    {6, 10, GREEN, 900},
    
    // Level 4 - Diamond pattern
    {7, 10, YELLOW, 1500},
    
    // Level 5 - Waves
    {7, 11, ORANGE, 2200},
    
    // Level 6 - Dense center
    {8, 11, RED, 3000},
    
    // Level 7 - Fortress walls
    {8, 12, PURPLE, 4000},
    
    // Level 8 - Zigzag chaos
    {9, 12, MAGENTA, 5200},
    
    // Level 9 - Spiral pattern
    {9, 13, MAROON, 6600},
    
    // Level 10 - Final Boss - Almost solid
    {10, 13, GOLD, 8200},
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

// Helper function to determine if a block should exist based on pattern
static bool ShouldBlockExist(int level, int row, int col, int totalRows, int totalCols) {
    switch(level) {
        case 0: // Level 1 - Simple full grid
            return true;
            
        case 1: // Level 2 - Pyramid from top
            {
                int centerCol = totalCols / 2;
                int allowedDistance = (row * totalCols) / (totalRows * 2) + 1;
                return abs(col - centerCol) <= allowedDistance;
            }
            
        case 2: // Level 3 - Checkerboard pattern
            return (row + col) % 2 == 0;
            
        case 3: // Level 4 - Diamond pattern
            {
                int centerRow = totalRows / 2;
                int centerCol = totalCols / 2;
                int distance = abs(row - centerRow) + abs(col - centerCol);
                int maxDistance = (totalRows + totalCols) / 3;
                return distance <= maxDistance;
            }
            
        case 4: // Level 5 - Wave pattern
            {
                int waveOffset = (int)(sin(col * 0.5f) * 2);
                return row >= waveOffset && row <= totalRows - 2;
            }
            
        case 5: // Level 6 - Dense center with gaps
            {
                int centerRow = totalRows / 2;
                int centerCol = totalCols / 2;
                int distRow = abs(row - centerRow);
                int distCol = abs(col - centerCol);
                // Denser in center, random gaps on edges
                if (distRow <= 2 && distCol <= 3) return true;
                return (row + col * 3) % 4 != 0;
            }
            
        case 6: // Level 7 - Fortress walls (thick sides, gap in middle)
            {
                bool isWall = col < 2 || col >= totalCols - 2;
                bool isTop = row < 3;
                bool isCenterStripe = abs(col - totalCols/2) == 1;
                return isWall || isTop || isCenterStripe;
            }
            
        case 7: // Level 8 - Zigzag chaos
            {
                int zigzag = (row % 3 == 0) ? (col % 3 != 1) : ((col + row) % 2 == 0);
                return zigzag;
            }
            
        case 8: // Level 9 - Spiral-ish pattern with gaps
            {
                int distFromEdge = (row < totalRows - row) ? row : totalRows - row - 1;
                distFromEdge = (distFromEdge < col) ? distFromEdge : col;
                distFromEdge = (distFromEdge < totalCols - col - 1) ? distFromEdge : totalCols - col - 1;
                return (row + col + distFromEdge) % 3 != 2;
            }
            
        case 9: // Level 10 - Almost solid (few strategic gaps)
            {
                // Small gaps to navigate through
                bool isGap = (row == 3 && col % 4 == 2) || 
                            (row == 6 && col % 5 == 1);
                return !isGap;
            }
            
        default:
            return true;
    }
}

void CreateBlocksForLevel(BlockCrusherGame* game, int screenWidth, int screenHeight) {
    Level* level = &levels[currentLevel];
    
    // Calculate block dimensions that fit within screen
    float availableWidth = screenWidth - resize(BLOCK_MARGIN * 2, BASE_WIDTH, screenWidth);
    float availableHeight = screenHeight - resize(BLOCK_TOP_MARGIN + PADDLE_Y_OFFSET + 100, BASE_HEIGHT, screenHeight);
    
    float blockSX = resize(BLOCK_MARGIN, BASE_WIDTH, screenWidth);
    float blockSY = resize(BLOCK_MARGIN, BASE_HEIGHT, screenHeight);
    
    // Calculate block size to fit all columns and rows
    float blockWidth = (availableWidth - (level->cols - 1) * blockSX) / level->cols;
    float blockHeight = (availableHeight - (level->rows - 1) * blockSY) / level->rows;
    
    // Clamp to reasonable sizes
    float maxBlockWidth = resize(BLOCK_WIDTH, BASE_WIDTH, screenWidth);
    float maxBlockHeight = resize(BLOCK_HEIGHT, BASE_HEIGHT, screenHeight);
    
    if (blockWidth > maxBlockWidth) blockWidth = maxBlockWidth;
    if (blockHeight > maxBlockHeight) blockHeight = maxBlockHeight;
    
    float blockTM = resize(BLOCK_TOP_MARGIN, BASE_HEIGHT, screenHeight);
    
    // First pass: count active blocks based on pattern
    int activeBlockCount = 0;
    for (int i = 0; i < level->rows; i++) {
        for (int j = 0; j < level->cols; j++) {
            if (ShouldBlockExist(currentLevel, i, j, level->rows, level->cols)) {
                activeBlockCount++;
            }
        }
    }
    
    game->blocksCount = activeBlockCount;
    
    if (game->blocks != NULL) {
        free(game->blocks);
    }
    
    game->blocks = (Block*)malloc(game->blocksCount * sizeof(Block));
    
    if (game->blocks == NULL) {
        TraceLog(LOG_ERROR, "Failed to allocate memory for blocks!");
        game->blocksCount = 0;
        return;
    }
    
    // Calculate hits required based on level
    int baseHits = 1;
    if (currentLevel >= 3) baseHits = 2;      // Levels 3-5: 2 hits
    if (currentLevel >= 6) baseHits = 3;      // Levels 6-8: 3 hits
    if (currentLevel >= 9) baseHits = 4;      // Levels 9-10: 4 hits
    
    // Center the block grid horizontally
    float totalGridWidth = level->cols * blockWidth + (level->cols - 1) * blockSX;
    float startX = (screenWidth - totalGridWidth) / 2;
    
    // Second pass: create blocks
    int blockIndex = 0;
    for (int i = 0; i < level->rows; i++) {
        for (int j = 0; j < level->cols; j++) {
            if (!ShouldBlockExist(currentLevel, i, j, level->rows, level->cols)) {
                continue;
            }
            
            game->blocks[blockIndex].rect = (Rectangle){
                startX + j * (blockWidth + blockSX), 
                i * (blockHeight + blockSY) + blockTM, 
                blockWidth, 
                blockHeight
            };
            game->blocks[blockIndex].active = true;
            
            // Add visual variety - tougher blocks have darker colors
            Color blockColor = level->blockColor;
            int hitsForThisBlock = baseHits;
            
            // Top rows are tougher in later levels
            if (currentLevel >= 5 && i < 2) {
                hitsForThisBlock = baseHits + 1;
                blockColor = ColorBrightness(level->blockColor, -0.3f);
            }
            // Special tough blocks in strategic positions
            else if (currentLevel >= 7) {
                // Make some blocks extra tough in later levels
                if ((i + j) % 5 == 0) {
                    hitsForThisBlock = baseHits + 1;
                    blockColor = ColorBrightness(level->blockColor, -0.25f);
                }
            }
            
            game->blocks[blockIndex].color = blockColor;
            game->blocks[blockIndex].hitsRequired = hitsForThisBlock;
            game->blocks[blockIndex].hitsTaken = 0;
            
            blockIndex++;
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
            // Beat the game! Award huge bonus
            game->score += LEVEL_SCORE_BONUS * 5;
            // Loop back to keep playing with harder difficulty
            currentLevel = 0;
            game->lives++; // Reward with extra life for beating all levels
        } else {
            // Progressive bonus - later levels give more points
            game->score += LEVEL_SCORE_BONUS * (1 + currentLevel / 3);
        }
        
        CreateBlocksForLevel(game, screenWidth, screenHeight);
    }
}