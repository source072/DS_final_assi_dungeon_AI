#include "Player.h"
#include <iostream>

Player::Player(const std::string& name)
    : name(name), money(50000), combatPower(10), currentRoomId(0), inventory() {}

std::string Player::getName()          const { return name; }
int         Player::getMoney()         const { return money; }
int         Player::getCombatPower()   const { return combatPower; }
int         Player::getCurrentRoomId() const { return currentRoomId; }

void Player::setCurrentRoomId(int roomId) { currentRoomId = roomId; }

void Player::addMoney(int amount) { money += amount; }

bool Player::spendMoney(int amount) {
    if (money < amount) return false;
    money -= amount;
    return true;
}

void Player::addCombatPower(int amount) { combatPower += amount; }

bool Player::isAlive() const { return money > 0; }

Inventory&       Player::getInventory()       { return inventory; }
const Inventory& Player::getInventory() const { return inventory; }

void Player::printStatus() const {
    std::cout << "\n=== 춘식이 상태 ===\n";
    std::cout << "  이름     : " << name        << "\n";
    std::cout << "  보유 금액: " << money        << "원\n";
    std::cout << "  전투력   : " << combatPower  << "\n";
    std::cout << "  아이템 수: " << inventory.size() << "개\n";
}
