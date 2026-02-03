#include "block_crusher_balls.h"
#include "block_crusher_collision.h"
#include <math.h>
#include <stdlib.h>

Ball balls[MAX_BALLS];
int activeBallCount = 1;

static inline float Vector2Distance(Vector2 v1, Vector2 v2) {
    float dx = v2.x - v1.x;
    float dy = v2.y - v1.y;
    return sqrtf(dx * dx + dy * dy);
}

void InitBalls(int screenWidth, int screenHeight) {
    balls[0].position = (Vector2){screenWidth/2, screenHeight/2};
    balls[0].speed = (Vector2){
        resize(BASE_BALL_SPEED, BASE_WIDTH, screenWidth), 
        resize(BASE_BALL_SPEED, BASE_HEIGHT, screenHeight)
    };
    balls[0].radius = resize(BALL_RADIUS, BASE_WIDTH, screenWidth);
    balls[0].active = true;
    balls[0].trailIndex = 0;
    balls[0].type = BALL_NORMAL;
    balls[0].specialTimer = 0.0f;
    
    for (int i = 0; i < BALL_TRAIL_LENGTH; i++) {
        balls[0].trail[i] = balls[0].position;
        balls[0].trailAlpha[i] = 0.0f;
        balls[0].trailSize[i] = balls[0].radius;
    }
    
    for (int i = 1; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }
    activeBallCount = 1;
}

static int GetAvailableBallIndex(void) {
    for (int i = 0; i < MAX_BALLS; i++) {
        if (!balls[i].active) return i;
    }
    return -1;
}

void DoubleBalls(void) {
    int currentActive = 0;
    for (int i = 0; i < MAX_BALLS; i++) {
        if (balls[i].active) currentActive++;
    }
    
    if (currentActive >= MAX_BALLS / 2) return;
    
    int ballsToCreate = currentActive;
    for (int i = 0; i < MAX_BALLS && ballsToCreate > 0; i++) {
        if (balls[i].active) {
            int newIndex = GetAvailableBallIndex();
            if (newIndex == -1) break;
            
            balls[newIndex] = balls[i];
            
            float angle = (rand() % 60 - 30) * (3.14159f / 180.0f); 
            float cos_a = cosf(angle);
            float sin_a = sinf(angle);
            
            float newSpeedX = balls[newIndex].speed.x * cos_a - balls[newIndex].speed.y * sin_a;
            float newSpeedY = balls[newIndex].speed.x * sin_a + balls[newIndex].speed.y * cos_a;
            
            balls[newIndex].speed.x = newSpeedX;
            balls[newIndex].speed.y = newSpeedY;
            
            balls[newIndex].trailIndex = 0;
            for (int k = 0; k < BALL_TRAIL_LENGTH; k++) {
                balls[newIndex].trail[k] = balls[newIndex].position;
                balls[newIndex].trailAlpha[k] = 0.0f;
                balls[newIndex].trailSize[k] = balls[newIndex].radius;
            }
            
            balls[newIndex].active = true;
            ballsToCreate--;
            activeBallCount++;
        }
    }
}

void SetBallType(int ballIndex, BallType type, float duration) {
    if (ballIndex < 0 || ballIndex >= MAX_BALLS || !balls[ballIndex].active) return;
    
    balls[ballIndex].type = type;
    balls[ballIndex].specialTimer = duration;
    
    switch (type) {
        case BALL_HEAVY:
            balls[ballIndex].radius *= 1.3f;
            break;
        case BALL_FIRE:
            balls[ballIndex].radius *= 1.1f;
            break;
        default:
            break;
    }
}

void ConvertAllBallsToType(BallType type, float duration) {
    for (int i = 0; i < MAX_BALLS; i++) {
        if (balls[i].active) {
            SetBallType(i, type, duration);
        }
    }
}

void ResetBall(int screenWidth, int screenHeight) {
    balls[0].position = (Vector2){screenWidth/2, screenHeight/2};
    balls[0].speed = (Vector2){
        resize(BASE_BALL_SPEED, BASE_WIDTH, screenWidth), 
        resize(BASE_BALL_SPEED, BASE_HEIGHT, screenHeight)
    };
    balls[0].active = true;
    balls[0].type = BALL_NORMAL;
    balls[0].specialTimer = 0.0f;
    
    for (int i = 0; i < BALL_TRAIL_LENGTH; i++) {
        balls[0].trail[i] = balls[0].position;
        balls[0].trailAlpha[i] = 0.0f;
        balls[0].trailSize[i] = balls[0].radius;
    }
    balls[0].trailIndex = 0;
    
    for (int i = 1; i < MAX_BALLS; i++) {
        balls[i].active = false;
    }
    activeBallCount = 1;
}

void UpdateBallTrail(Ball *ball) {
    ball->trail[ball->trailIndex] = ball->position;
    ball->trailAlpha[ball->trailIndex] = 1.0f;
    ball->trailSize[ball->trailIndex] = ball->radius;
    ball->trailIndex = (ball->trailIndex + 1) % BALL_TRAIL_LENGTH;
    
    for (int i = 0; i < BALL_TRAIL_LENGTH; i++) {
        if (ball->trailAlpha[i] > 0.0f) {
            ball->trailAlpha[i] -= 1.0f / (BALL_TRAIL_LENGTH * 0.8f);
            ball->trailSize[i] -= ball->radius / (BALL_TRAIL_LENGTH * 0.9f);
            
            if (ball->trailAlpha[i] < 0.0f) ball->trailAlpha[i] = 0.0f;
            if (ball->trailSize[i] < 1.0f) ball->trailSize[i] = 1.0f;
        }
    }
}

void UpdateBalls(BlockCrusherGame* game, int screenWidth, int screenHeight) {
    float baseRadius = resize(BALL_RADIUS, BASE_WIDTH, screenWidth);
    
    for (int ballIdx = 0; ballIdx < MAX_BALLS; ballIdx++) {
        if (!balls[ballIdx].active) continue;
        
        if (balls[ballIdx].specialTimer > 0.0f) {
            balls[ballIdx].specialTimer -= GetFrameTime();
            if (balls[ballIdx].specialTimer <= 0.0f) {
                balls[ballIdx].type = BALL_NORMAL;
                balls[ballIdx].radius = baseRadius;
            }
        }
        
        UpdateBallTrail(&balls[ballIdx]);
        
        balls[ballIdx].position.x += balls[ballIdx].speed.x;
        balls[ballIdx].position.y += balls[ballIdx].speed.y;
        
        if (balls[ballIdx].type == BALL_FIRE) {
            float damageRadius = balls[ballIdx].radius * 2.5f;
            for (int i = 0; i < game->blocksCount; i++) {
                if (!game->blocks[i].active) continue;
                
                Vector2 blockCenter = {
                    game->blocks[i].rect.x + game->blocks[i].rect.width / 2,
                    game->blocks[i].rect.y + game->blocks[i].rect.height / 2
                };
                
                float dist = Vector2Distance(balls[ballIdx].position, blockCenter);
                if (dist < damageRadius) {
                    game->blocks[i].hitsTaken++;
                    if (game->blocks[i].hitsTaken >= game->blocks[i].hitsRequired) {
                        game->blocks[i].active = false;
                        game->score += 10;
                    }
                }
            }
        }
        
        if (balls[ballIdx].position.x >= screenWidth - balls[ballIdx].radius || 
            balls[ballIdx].position.x <= balls[ballIdx].radius) {
            balls[ballIdx].speed.x *= -1;
            balls[ballIdx].position.x = fmaxf(balls[ballIdx].radius, 
                                             fminf(screenWidth - balls[ballIdx].radius, 
                                                   balls[ballIdx].position.x));
        }
        
        if (balls[ballIdx].position.y <= balls[ballIdx].radius) {
            balls[ballIdx].speed.y *= -1;
            balls[ballIdx].position.y = balls[ballIdx].radius + 1;
        }
        
        if (balls[ballIdx].position.y >= screenHeight + balls[ballIdx].radius) {
            balls[ballIdx].active = false;
            activeBallCount--;
            
            if (activeBallCount <= 0) {
                game->lives--;
                if (game->lives <= 0) {
                    game->gameOver = true;
                } else {
                    ResetBall(screenWidth, screenHeight);
                }
            }
        }
        
        HandleBallPaddleCollision(ballIdx, &game->paddle);
        
        bool hitBlock = false;
        for (int i = 0; i < game->blocksCount; i++) {
            if (!game->blocks[i].active) continue;
            
            if (CheckCollisionCircleRec(balls[ballIdx].position, balls[ballIdx].radius, game->blocks[i].rect)) {
                HandleBallBlockCollision(ballIdx, i, game);
                hitBlock = true;
                
                if (balls[ballIdx].type != BALL_LASER) {
                    break;
                }
            }
        }
    }
}

static Color GetBallColor(BallType type) {
    switch (type) {
        case BALL_EXPLOSIVE:  return ORANGE;
        case BALL_LASER:      return (Color){0, 255, 255, 255};   // cyan
        case BALL_HEAVY:      return (Color){128, 128, 128, 255}; // gray
        case BALL_FIRE:       return RED;
        default:              return WHITE;
    }
}

void DrawBallWithTrail(Ball *ball) {
    Color trailColor = GetBallColor(ball->type);

    for (int i = BALL_TRAIL_LENGTH - 1; i >= 0; i--) {
        int index = (ball->trailIndex + i) % BALL_TRAIL_LENGTH;
        
        if (ball->trailAlpha[index] > 0.0f) {
            float easedAlpha = sqrtf(ball->trailAlpha[index]);
            Color fadeColor = Fade(trailColor, easedAlpha * 0.6f);
            
            DrawCircleV(ball->trail[index], ball->trailSize[index], fadeColor);
            
            if (ball->trailAlpha[index] > 0.5f) {
                DrawCircleV(ball->trail[index], ball->trailSize[index] * 1.3f, 
                           Fade(trailColor, easedAlpha * 0.2f));
            }
        }
    }
    
    Color ballColor = GetBallColor(ball->type);
    
    switch (ball->type) {
        case BALL_EXPLOSIVE:
            // pulsing orange effect
            {
                float pulse = sinf(GetTime() * 10.0f) * 0.2f + 0.8f;
                DrawCircleV(ball->position, ball->radius * 1.5f * pulse, Fade(ORANGE, 0.3f));
            }
            break;
            
        case BALL_LASER:
            // cyan glow with sparks effect
            DrawCircleV(ball->position, ball->radius * 1.4f, Fade((Color){0, 255, 255, 255}, 0.4f));
            break;
            
        case BALL_HEAVY:
            // dark effect
            DrawCircleV(ball->position, ball->radius * 1.2f, Fade((Color){64, 64, 64, 255}, 0.5f));
            break;
            
        case BALL_FIRE:
            // fire particles
            {
                float pulse = sinf(GetTime() * 8.0f) * 0.3f + 0.7f;
                DrawCircleV(ball->position, ball->radius * 1.6f * pulse, Fade(RED, 0.4f));
                DrawCircleV(ball->position, ball->radius * 1.3f * pulse, Fade(ORANGE, 0.3f));
            }
            break;
            
        default:
            break;
    }
    
    DrawCircleV(ball->position, ball->radius, ballColor);
    DrawCircleV(ball->position, ball->radius * 1.1f, Fade(ballColor, 0.3f));
    
    DrawCircle(ball->position.x - ball->radius/3, 
               ball->position.y - ball->radius/3, 
               ball->radius/4, Fade(WHITE, 0.8f));
}

void DrawBalls(void) {
    for (int i = 0; i < MAX_BALLS; i++) {
        if (balls[i].active) {
            DrawBallWithTrail(&balls[i]);
        }
    }
}