#ifndef PLAYER_H
#define PLAYER_H

#include "ds/Inventory.h"
#include <string>

class Player {
private:
    std::string name;
    int         money;
    int         combatPower;
    int         currentRoomId;
    Inventory   inventory;

public:
    Player(const std::string& name = "춘식");

    std::string getName()        const;
    int         getMoney()       const;
    int         getCombatPower() const;
    int         getCurrentRoomId() const;

    void setCurrentRoomId(int roomId);
    void addMoney(int amount);
    bool spendMoney(int amount);   // 돈 부족하면 false 반환
    void addCombatPower(int amount);

    bool isAlive() const;          // money > 0

    Inventory&       getInventory();
    const Inventory& getInventory() const;

    void printStatus() const;
};

#endif
