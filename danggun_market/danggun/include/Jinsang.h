#ifndef JINSANG_H
#define JINSANG_H

#include <string>

enum class JinsangSkill {
    Eojji,       // 억지부리기: 확률 -10%
    Hwanbul,     // 환불요구:   확률 -15% + 돈 탈취
    Nego,        // 네고요?:    확률 -20%
    Dokjeom      // 시장독점:   확률 -25% + 돈 탈취
};

struct SkillEffect {
    int  probDelta;   // 확률 변화 (음수)
    int  moneySteal;  // 탈취 금액 (0이면 없음)
    std::string message;
};

class Jinsang {
private:
    std::string name;
    int         combatPower;
    int         stage;
    // 보유 스킬 목록 (최대 3개)
    JinsangSkill skills[3];
    int          skillCount;

public:
    Jinsang();
    Jinsang(const std::string& name, int combatPower, int stage,
            JinsangSkill* skillList, int skillCount);

    std::string  getName()        const;
    int          getCombatPower() const;
    int          getStage()       const;

    // 랜덤 스킬 1개 선택 후 효과 반환
    SkillEffect  useRandomSkill() const;

    void print() const;
};

#endif
