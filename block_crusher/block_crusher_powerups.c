#include "block_crusher_powerups.h"
#include "block_crusher_balls.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

PowerUp powerups[MAX_POWERUPS];
float paddleEnlargeTimer = 0.0f;
float slowBallTimer = 0.0f;

static int GetAvailablePowerUpIndex(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) return i;
    }
    return -1;
}

void InitPowerUps(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        powerups[i].active = false;
    }
    paddleEnlargeTimer = 0.0f;
    slowBallTimer = 0.0f;
}

void SpawnPowerUp(float x, float y, int screenWidth, int screenHeight) {
    int index = GetAvailablePowerUpIndex();
    if (index == -1) return;
    
    int randVal = rand() % 100;
    PowerUpType type;
    
    if (randVal < 40) type = POWERUP_DOUBLE_BALLS;
    else if (randVal < 70) type = POWERUP_ENLARGE_PADDLE;
    else if (randVal < 90) type = POWERUP_SLOW_BALL;
    else type = POWERUP_EXTRA_LIFE;
    
    powerups[index].position = (Vector2){x, y};
    powerups[index].speed = (Vector2){0, resize(POWERUP_SPEED, BASE_HEIGHT, screenHeight)};
    powerups[index].radius = resize(POWERUP_RADIUS, BASE_WIDTH, screenWidth);
    powerups[index].active = true;
    powerups[index].type = type;
    powerups[index].spawnTime = GetTime();
}

void ActivatePowerUp(PowerUpType type, Paddle* paddle) {
    switch (type) {
        case POWERUP_DOUBLE_BALLS:
            DoubleBalls();
            break;
            
        case POWERUP_ENLARGE_PADDLE:
            paddleEnlargeTimer = 10.0f;
            paddle->rect.width *= 1.5f;
            break;
            
        case POWERUP_SLOW_BALL:
            slowBallTimer = 8.0f;
            for (int i = 0; i < MAX_BALLS; i++) {
                if (balls[i].active) {
                    balls[i].speed.x *= 0.5f;
                    balls[i].speed.y *= 0.5f;
                }
            }
            break;
            
        case POWERUP_EXTRA_LIFE:
            // Life increment handled by caller
            break;
            
        default:
            break;
    }
}

void UpdatePowerUps(BlockCrusherGame* game, int screenWidth, int screenHeight) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        if (!powerups[i].active) continue;
        
        powerups[i].position.y += powerups[i].speed.y;
        
        if (CheckCollisionCircleRec(powerups[i].position, powerups[i].radius, game->paddle.rect)) {
            if (powerups[i].type == POWERUP_EXTRA_LIFE) {
                game->lives++;
            } else {
                ActivatePowerUp(powerups[i].type, &game->paddle);
            }
            powerups[i].active = false;
            game->score += 50;
        }
        
        if (powerups[i].position.y > screenHeight + powerups[i].radius) {
            powerups[i].active = false;
        }
    }
}

void UpdatePowerUpTimers(Paddle* paddle) {
    float deltaTime = GetFrameTime();
    
    if (paddleEnlargeTimer > 0) {
        paddleEnlargeTimer -= deltaTime;
        if (paddleEnlargeTimer <= 0) {
            paddle->rect.width = paddle->baseWidth;
        }
    }
    
    if (slowBallTimer > 0) {
        slowBallTimer -= deltaTime;
        if (slowBallTimer <= 0) {
            for (int i = 0; i < MAX_BALLS; i++) {
                if (balls[i].active) {
                    balls[i].speed.x /= 0.5f;
                    balls[i].speed.y /= 0.5f;
                }
            }
        }
    }
}

static Color GetPowerUpColor(PowerUpType type) {
    switch (type) {
        case POWERUP_DOUBLE_BALLS:    return BLUE;
        case POWERUP_ENLARGE_PADDLE:  return GREEN;
        case POWERUP_SLOW_BALL:       return YELLOW;
        case POWERUP_EXTRA_LIFE:      return RED;
        default:                      return WHITE;
    }
}

static const char* GetPowerUpSymbol(PowerUpType type) {
    switch (type) {
        case POWERUP_DOUBLE_BALLS:    return "2x";
        case POWERUP_ENLARGE_PADDLE:  return "++";
        case POWERUP_SLOW_BALL:       return "--";
        case POWERUP_EXTRA_LIFE:      return "+1";
        default:                      return "?";
    }
}

static void DrawPowerUp(PowerUp* pu) {
    if (!pu->active) return;
    
    Color mainColor = GetPowerUpColor(pu->type);
    
    float pulse = sinf(GetTime() * 5.0f) * 0.1f + 0.9f;
    float radius = pu->radius * pulse;
    
    DrawCircleV(pu->position, radius, mainColor);
    DrawCircleV(pu->position, radius * 1.3f, Fade(mainColor, 0.3f));
    
    int fontSize = radius;
    const char* symbol = GetPowerUpSymbol(pu->type);
    int textWidth = MeasureText(symbol, fontSize);
    DrawText(symbol, 
             pu->position.x - textWidth/2, 
             pu->position.y - fontSize/2, 
             fontSize, WHITE);
}

void DrawPowerUps(void) {
    for (int i = 0; i < MAX_POWERUPS; i++) {
        DrawPowerUp(&powerups[i]);
    }
}

void ClosePowerUps(void) {
    InitPowerUps();
}