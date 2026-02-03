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
    
    if (randVal < 20) type = POWERUP_DOUBLE_BALLS;
    else if (randVal < 35) type = POWERUP_ENLARGE_PADDLE;
    else if (randVal < 50) type = POWERUP_SLOW_BALL;
    else if (randVal < 60) type = POWERUP_EXTRA_LIFE;
    else if (randVal < 70) type = POWERUP_EXPLOSIVE_BALL;
    else if (randVal < 80) type = POWERUP_LASER_BALL;
    else if (randVal < 90) type = POWERUP_HEAVY_BALL;
    else type = POWERUP_FIRE_BALL;
    
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
            break;
            
        case POWERUP_EXPLOSIVE_BALL:
            ConvertAllBallsToType(BALL_EXPLOSIVE, 15.0f);
            break;
            
        case POWERUP_LASER_BALL:
            ConvertAllBallsToType(BALL_LASER, 12.0f);
            break;
            
        case POWERUP_HEAVY_BALL:
            ConvertAllBallsToType(BALL_HEAVY, 10.0f);
            break;
            
        case POWERUP_FIRE_BALL:
            ConvertAllBallsToType(BALL_FIRE, 8.0f);
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
        case POWERUP_EXPLOSIVE_BALL:  return ORANGE;
        case POWERUP_LASER_BALL:      return (Color){0, 255, 255, 255}; // cyan
        case POWERUP_HEAVY_BALL:      return (Color){128, 128, 128, 255}; // gray
        case POWERUP_FIRE_BALL:       return (Color){255, 100, 0, 255}; // bright orange-red
        default:                      return WHITE;
    }
}

static const char* GetPowerUpSymbol(PowerUpType type) {
    switch (type) {
        case POWERUP_DOUBLE_BALLS:    return "2x";
        case POWERUP_ENLARGE_PADDLE:  return "++";
        case POWERUP_SLOW_BALL:       return "--";
        case POWERUP_EXTRA_LIFE:      return "+1";
        case POWERUP_EXPLOSIVE_BALL:  return "EX";
        case POWERUP_LASER_BALL:      return "LZ";
        case POWERUP_HEAVY_BALL:      return "HV";
        case POWERUP_FIRE_BALL:       return "FR";
        default:                      return "?";
    }
}

static void DrawPowerUp(PowerUp* pu) {
    if (!pu->active) return;
    
    Color mainColor = GetPowerUpColor(pu->type);
    
    float pulse = sinf(GetTime() * 5.0f) * 0.1f + 0.9f;
    float radius = pu->radius * pulse;
    
    switch (pu->type) {
        case POWERUP_EXPLOSIVE_BALL:
            {
                float explosionPulse = sinf(GetTime() * 8.0f) * 0.3f + 0.7f;
                DrawCircleV(pu->position, radius * 1.8f * explosionPulse, Fade(ORANGE, 0.2f));
            }
            break;
            
        case POWERUP_LASER_BALL:
            DrawCircleV(pu->position, radius * 1.5f, Fade((Color){0, 255, 255, 255}, 0.3f));
            break;
            
        case POWERUP_FIRE_BALL:
            {
                float firePulse = sinf(GetTime() * 10.0f) * 0.4f + 0.6f;
                DrawCircleV(pu->position, radius * 1.7f * firePulse, Fade(RED, 0.25f));
                DrawCircleV(pu->position, radius * 1.4f * firePulse, Fade(ORANGE, 0.3f));
            }
            break;
            
        default:
            break;
    }
    
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