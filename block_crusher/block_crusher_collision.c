#include "block_crusher_collision.h"
#include "block_crusher_powerups.h"
#include <math.h>
#include <stdlib.h>

void HandleBallBlockCollision(int ballIdx, int blockIdx, BlockCrusherGame* game) {
    if (!game->blocks[blockIdx].active) return;
    
    Rectangle block = game->blocks[blockIdx].rect;
    Vector2 ballPos = balls[ballIdx].position;
    float radius = balls[ballIdx].radius;
    
    if (!CheckCollisionCircleRec(ballPos, radius, block)) return;
    
    game->blocks[blockIdx].hitsTaken++;
    
    if (game->blocks[blockIdx].hitsTaken >= game->blocks[blockIdx].hitsRequired) {
        game->blocks[blockIdx].active = false;
        game->score += 10 * game->blocks[blockIdx].hitsRequired;
        
        if (rand() % 100 < POWERUP_DROP_CHANCE) {
            float centerX = block.x + block.width / 2;
            float centerY = block.y + block.height / 2;
            SpawnPowerUp(centerX, centerY, GetScreenWidth(), GetScreenHeight());
        }
    }
    
    float overlapLeft = (ballPos.x + radius) - block.x;
    float overlapRight = (block.x + block.width) - (ballPos.x - radius);
    float overlapTop = (ballPos.y + radius) - block.y;
    float overlapBottom = (block.y + block.height) - (ballPos.y - radius);
    
    float minOverlap = fminf(fminf(overlapLeft, overlapRight), 
                           fminf(overlapTop, overlapBottom));
    
    if (minOverlap == overlapLeft || minOverlap == overlapRight) {
        balls[ballIdx].speed.x *= -1;
        balls[ballIdx].position.x += (minOverlap == overlapLeft) ? -minOverlap : minOverlap;
    } else {
        balls[ballIdx].speed.y *= -1;
        balls[ballIdx].position.y += (minOverlap == overlapTop) ? -minOverlap : minOverlap;
    }
    
    balls[ballIdx].speed.x += ((rand() % 100) / 1000.0f - 0.05f);
    balls[ballIdx].speed.y += ((rand() % 100) / 1000.0f - 0.05f);
    
    float speed = sqrtf(balls[ballIdx].speed.x * balls[ballIdx].speed.x + 
                       balls[ballIdx].speed.y * balls[ballIdx].speed.y);
    float maxSpeed = resize(BASE_BALL_SPEED * 2.5f, BASE_WIDTH, GetScreenWidth());
    float minSpeed = resize(BASE_BALL_SPEED * 0.5f, BASE_WIDTH, GetScreenWidth());
    
    if (speed > maxSpeed) {
        balls[ballIdx].speed.x = (balls[ballIdx].speed.x / speed) * maxSpeed;
        balls[ballIdx].speed.y = (balls[ballIdx].speed.y / speed) * maxSpeed;
    } else if (speed < minSpeed) {
        balls[ballIdx].speed.x = (balls[ballIdx].speed.x / speed) * minSpeed;
        balls[ballIdx].speed.y = (balls[ballIdx].speed.y / speed) * minSpeed;
    }
}

void HandleBallPaddleCollision(int ballIdx, Paddle* paddle) {
    if (!CheckCollisionCircleRec(balls[ballIdx].position, balls[ballIdx].radius, paddle->rect)) {
        return;
    }
    
    balls[ballIdx].speed.y *= -1;
    
    float hitPosition = (balls[ballIdx].position.x - paddle->rect.x) / paddle->rect.width;
    float angle = (hitPosition - 0.5f) * 1.5f;
    
    float speed = sqrtf(balls[ballIdx].speed.x * balls[ballIdx].speed.x + 
                       balls[ballIdx].speed.y * balls[ballIdx].speed.y);
    
    balls[ballIdx].speed.x = sinf(angle) * speed;
    balls[ballIdx].speed.y = -fabsf(cosf(angle) * speed);
    
    balls[ballIdx].position.y = paddle->rect.y - balls[ballIdx].radius - 1;
}

void UpdatePaddle(Paddle* paddle, int screenWidth) {
    float pd_speed = resize(BASE_PADDLE_SPEED, BASE_WIDTH, screenWidth);
    
    if (IsKeyDown(KEY_LEFT) && paddle->rect.x > 0) {
        paddle->rect.x -= pd_speed;
    }
    if (IsKeyDown(KEY_RIGHT) && paddle->rect.x < screenWidth - paddle->rect.width) {
        paddle->rect.x += pd_speed;
    }
    
    if (paddle->rect.x < 0) paddle->rect.x = 0;
    if (paddle->rect.x > screenWidth - paddle->rect.width) {
        paddle->rect.x = screenWidth - paddle->rect.width;
    }
}