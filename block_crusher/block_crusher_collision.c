#include "block_crusher_collision.h"
#include "block_crusher_balls.h"
#include "block_crusher_powerups.h"
#include <math.h>
#include <stdlib.h>

static inline float Vector2Distance(Vector2 v1, Vector2 v2) {
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return sqrtf(dx * dx + dy * dy);
}

void HandleBallPaddleCollision(int ballIndex, Paddle* paddle) {
    if (!balls[ballIndex].active || !paddle->active) return;
    
    if (CheckCollisionCircleRec(balls[ballIndex].position, balls[ballIndex].radius, paddle->rect)) {
        if (balls[ballIndex].speed.y > 0) {
            balls[ballIndex].speed.y *= -1;
            
            float paddleCenter = paddle->rect.x + paddle->rect.width / 2;
            float hitPosition = (balls[ballIndex].position.x - paddleCenter) / (paddle->rect.width / 2);
            hitPosition = fmaxf(-1.0f, fminf(1.0f, hitPosition));
            
            float angleAdjustment = hitPosition * 45.0f * (3.14159f / 180.0f);
            float speed = sqrtf(balls[ballIndex].speed.x * balls[ballIndex].speed.x + 
                              balls[ballIndex].speed.y * balls[ballIndex].speed.y);
            
            balls[ballIndex].speed.x = speed * sinf(angleAdjustment);
            balls[ballIndex].speed.y = -fabsf(speed * cosf(angleAdjustment));
            
            balls[ballIndex].position.y = paddle->rect.y - balls[ballIndex].radius - 1;
        }
    }
}

static void ExplodeBlocks(BlockCrusherGame* game, Vector2 epicenter, float radius) {
    for (int i = 0; i < game->blocksCount; i++) {
        if (!game->blocks[i].active) continue;
        
        Vector2 blockCenter = {
            game->blocks[i].rect.x + game->blocks[i].rect.width / 2,
            game->blocks[i].rect.y + game->blocks[i].rect.height / 2
        };
        
        float distance = Vector2Distance(epicenter, blockCenter);
        
        if (distance < radius) {
            game->blocks[i].active = false;
            game->score += 10;
            
            if ((rand() % 100) < 5) {
                SpawnPowerUp(blockCenter.x, blockCenter.y, 800, 600);
            }
        }
    }
}

void HandleBallBlockCollision(int ballIndex, int blockIndex, BlockCrusherGame* game) {
    if (!balls[ballIndex].active || !game->blocks[blockIndex].active) return;
    
    Ball* ball = &balls[ballIndex];
    Block* block = &game->blocks[blockIndex];
    
    switch (ball->type) {
        case BALL_EXPLOSIVE:
            {
                Vector2 explosionCenter = {
                    block->rect.x + block->rect.width / 2,
                    block->rect.y + block->rect.height / 2
                };
                float explosionRadius = ball->radius * 6.0f;
                ExplodeBlocks(game, explosionCenter, explosionRadius);
                
                game->consecutiveHits = 0;
            }
            break;
            
        case BALL_LASER:
            block->active = false;
            game->score += 10;
            game->consecutiveHits++;
            
            if ((rand() % 100) < POWERUP_DROP_CHANCE) {
                SpawnPowerUp(block->rect.x + block->rect.width/2, 
                           block->rect.y + block->rect.height/2,
                           800, 600);
            }
            
            if (game->consecutiveHits >= 5) {
                game->score += game->consecutiveHits * 5;
            }
            return;
            
        case BALL_HEAVY:
            block->active = false;
            game->score += 15;
            game->consecutiveHits++;
            
            if ((rand() % 100) < POWERUP_DROP_CHANCE) {
                SpawnPowerUp(block->rect.x + block->rect.width/2, 
                           block->rect.y + block->rect.height/2,
                           800, 600);
            }
            break;
            
        case BALL_FIRE:
            block->hitsTaken++;
            if (block->hitsTaken >= block->hitsRequired) {
                block->active = false;
                game->score += 10;
                game->consecutiveHits++;
                
                if ((rand() % 100) < POWERUP_DROP_CHANCE) {
                    SpawnPowerUp(block->rect.x + block->rect.width/2, 
                               block->rect.y + block->rect.height/2,
                               800, 600);
                }
            } else {
                block->color = ColorBrightness(block->color, -0.2f);
            }
            break;
            
        case BALL_NORMAL:
        default:
            block->hitsTaken++;
            
            if (block->hitsTaken >= block->hitsRequired) {
                block->active = false;
                game->score += 10;
                game->consecutiveHits++;
                
                if ((rand() % 100) < POWERUP_DROP_CHANCE) {
                    SpawnPowerUp(block->rect.x + block->rect.width/2, 
                               block->rect.y + block->rect.height/2,
                               800, 600);
                }
                
                if (game->consecutiveHits >= 10) {
                    game->score += 100;
                    int specialType = POWERUP_EXPLOSIVE_BALL + (rand() % 4);
                    PowerUp* pu = &powerups[0];
                    for (int i = 0; i < MAX_POWERUPS; i++) {
                        if (!powerups[i].active) {
                            pu = &powerups[i];
                            break;
                        }
                    }
                    pu->position = (Vector2){block->rect.x + block->rect.width/2, 
                                            block->rect.y + block->rect.height/2};
                    pu->speed = (Vector2){0, 3};
                    pu->radius = 10;
                    pu->active = true;
                    pu->type = specialType;
                    pu->spawnTime = GetTime();
                }
            } else {
                block->color = ColorBrightness(block->color, -0.15f);
                game->consecutiveHits = 0;
            }
            break;
    }
    
    Vector2 blockCenter = {block->rect.x + block->rect.width / 2, 
                          block->rect.y + block->rect.height / 2};
    
    float dx = ball->position.x - blockCenter.x;
    float dy = ball->position.y - blockCenter.y;
    
    if (fabsf(dx) > fabsf(dy)) {
        ball->speed.x *= -1;
        ball->position.x = (dx > 0) ? block->rect.x + block->rect.width + ball->radius + 1
                                    : block->rect.x - ball->radius - 1;
    } else {
        ball->speed.y *= -1;
        ball->position.y = (dy > 0) ? block->rect.y + block->rect.height + ball->radius + 1
                                    : block->rect.y - ball->radius - 1;
    }
}