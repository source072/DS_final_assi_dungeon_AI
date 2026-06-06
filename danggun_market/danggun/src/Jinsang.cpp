#include "Jinsang.h"
#include <iostream>
#include <cstdlib>

Jinsang::Jinsang() : name(""), combatPower(0), stage(0), skillCount(0) {}

Jinsang::Jinsang(const std::string& name, int combatPower, int stage,
                 JinsangSkill* skillList, int cnt)
    : name(name), combatPower(combatPower), stage(stage), skillCount(cnt) {
    for (int i = 0; i < cnt && i < 3; ++i)
        skills[i] = skillList[i];
}

std::string Jinsang::getName()        const { return name; }
int         Jinsang::getCombatPower() const { return combatPower; }
int         Jinsang::getStage()       const { return stage; }

SkillEffect Jinsang::useRandomSkill() const {
    int idx = std::rand() % skillCount;
    switch (skills[idx]) {
        case JinsangSkill::Eojji:
            return { -10, 0,     "억지부리기! 거래 확률 -10%" };
        case JinsangSkill::Hwanbul:
            return { -15, 5000,  "환불 요구! 확률 -15%, 돈 5,000원 탈취!" };
        case JinsangSkill::Nego:
            return { -20, 0,     "네고요? 거래 확률 -20%" };
        case JinsangSkill::Dokjeom:
            return { -25, 30000, "시장 독점! 확률 -25%, 돈 30,000원 탈취!" };
        default:
            return { 0, 0, "진상이 멀뚱히 쳐다본다..." };
    }
}

void Jinsang::print() const {
    std::cout << "[진상] " << name
              << " (전투력: " << combatPower
              << ", Stage " << stage << ")\n";
}
