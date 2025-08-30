#include "game_manager.h"

int main(void) {
    GameManager gameManager;
    InitGameManager(&gameManager);
    RunGameManager(&gameManager);
    CloseGameManager(&gameManager);
    
    return 0;
}