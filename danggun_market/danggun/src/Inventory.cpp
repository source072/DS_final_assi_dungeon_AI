#include "ds/Inventory.h"
#include <iostream>

Inventory::Inventory() : head(nullptr), count(0) {}

Inventory::~Inventory() { clear(); }

// 앞쪽 삽입 (O(1))
void Inventory::addItem(const Item& item) {
    Node* node  = new Node(item);
    node->next  = head;
    head        = node;
    ++count;
}

// 이름으로 삭제 (O(n))
bool Inventory::removeItem(const std::string& itemName) {
    Node* current = head;
    Node* prev    = nullptr;
    while (current != nullptr) {
        if (current->item.getName() == itemName) {
            if (prev == nullptr) {
                head = current->next;       // head 제거
            } else {
                prev->next = current->next; // 중간/꼬리 제거
            }
            delete current;
            --count;
            return true;
        }
        prev    = current;
        current = current->next;
    }
    return false;
}

Item* Inventory::findItem(const std::string& itemName) {
    Node* current = head;
    while (current != nullptr) {
        if (current->item.getName() == itemName)
            return &current->item;
        current = current->next;
    }
    return nullptr;
}

const Item* Inventory::findItem(const std::string& itemName) const {
    Node* current = head;
    while (current != nullptr) {
        if (current->item.getName() == itemName)
            return &current->item;
        current = current->next;
    }
    return nullptr;
}

bool Inventory::hasItemOfType(ItemType type) const {
    Node* current = head;
    while (current != nullptr) {
        if (current->item.getType() == type) return true;
        current = current->next;
    }
    return false;
}

bool Inventory::isEmpty() const { return count == 0; }
int  Inventory::size()    const { return count; }

void Inventory::clear() {
    Node* current = head;
    while (current != nullptr) {
        Node* next = current->next;
        delete current;
        current = next;
    }
    head  = nullptr;
    count = 0;
}

void Inventory::print() const {
    if (head == nullptr) {
        std::cout << "인벤토리가 비어 있습니다.\n";
        return;
    }
    std::cout << "=== 인벤토리 (" << count << "개) ===\n";
    Node* current = head;
    while (current != nullptr) {
        std::cout << "  - ";
        current->item.print();
        current = current->next;
    }
}
