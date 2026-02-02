#ifndef BLOCK_CRUSHER_POWERUPS_H
#define BLOCK_CRUSHER_POWERUPS_H

#include "block_crusher.h"

#define MAX_POWERUPS 15
#define POWERUP_DROP_CHANCE 15

typedef enum {
    POWERUP_DOUBLE_BALLS,
    POWERUP_ENLARGE_PADDLE,
    POWERUP_SLOW_BALL,
    POWERUP_EXTRA_LIFE,
    POWERUP_COUNT
} PowerUpType;

typedef struct {
    Vector2 position;
    Vector2 speed;
    bool active;
    float radius;
    PowerUpType type;
    float spawnTime;
} PowerUp;

// Extern declarations
extern PowerUp powerups[MAX_POWERUPS];
extern float paddleEnlargeTimer;
extern float slowBallTimer;

void InitPowerUps(void);
void SpawnPowerUp(float x, float y, int screenWidth, int screenHeight);
void ActivatePowerUp(PowerUpType type, Paddle* paddle);
void UpdatePowerUps(BlockCrusherGame* game, int screenWidth, int screenHeight);
void UpdatePowerUpTimers(Paddle* paddle);
void DrawPowerUps(void);
void ClosePowerUps(void);

#endif