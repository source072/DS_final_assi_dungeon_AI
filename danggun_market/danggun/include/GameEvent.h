#ifndef GAME_EVENT_H
#define GAME_EVENT_H

#include <string>

struct GameEvent {
    std::string description;
    int         probDelta;   // 거래 확률 변화
    int         moneyDelta;  // 돈 변화

    GameEvent(const std::string& description = "", int probDelta = 0, int moneyDelta = 0)
        : description(description), probDelta(probDelta), moneyDelta(moneyDelta) {}
};

#endif
