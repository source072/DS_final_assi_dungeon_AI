#include "Game.h"
#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);  // 출력 UTF-8
    SetConsoleCP(65001);        // 입력 UTF-8
#endif
    Game game;
    game.run();
    return 0;
}
