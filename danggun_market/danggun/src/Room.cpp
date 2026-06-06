#include "Room.h"
#include <iostream>

Room::Room() : id(-1), name(""), description(""), items(), visited(false) {}

Room::Room(int id, const std::string& name, const std::string& description)
    : id(id), name(name), description(description), items(), visited(false) {}

int         Room::getId()          const { return id; }
std::string Room::getName()        const { return name; }
std::string Room::getDescription() const { return description; }

void Room::setVisited(bool value) { visited = value; }
bool Room::hasBeenVisited()  const { return visited; }

void Room::addItem(const Item& item) { items.pushBack(item); }

bool Room::takeItem(const std::string& itemName, Item& output) {
    for (int i = 0; i < items.size(); ++i) {
        if (items[i].getName() == itemName) {
            output = items[i];
            items.removeAt(i);
            return true;
        }
    }
    return false;
}

int         Room::itemCount()          const { return items.size(); }
const Item& Room::getItem(int index)   const { return items[index]; }

void Room::printDescription() const {
    std::cout << "\n=============================\n";
    std::cout << "  " << name << "\n";
    std::cout << "=============================\n";
    std::cout << description << "\n";
    if (!items.isEmpty()) {
        std::cout << "[아이템]\n";
        for (int i = 0; i < items.size(); ++i) {
            std::cout << "  - ";
            items[i].print();
        }
    }
}
