#ifndef ITEM_H
#define ITEM_H

#include <string>

enum class ItemType {
    Weapon,   // 전투력 영구 상승
    Food,     // 전투 중 확률 상승 (소모)
    Pet,      // 카지노 꽝 확률 감소 + 전투 확률 상승
    Goods,    // 리셀가 상승 + 물건자랑 가능
    None
};

class Item {
private:
    std::string name;
    std::string description;
    int         value;       // 구매 가격
    int         effectValue; // 전투력 상승량 or 확률 상승량
    ItemType    type;

public:
    Item();
    Item(const std::string& name, const std::string& description,
         int value, int effectValue = 0, ItemType type = ItemType::None);

    std::string getName()        const;
    std::string getDescription() const;
    int         getValue()       const;
    int         getEffectValue() const;
    ItemType    getType()        const;

    void print() const;
};

#endif
