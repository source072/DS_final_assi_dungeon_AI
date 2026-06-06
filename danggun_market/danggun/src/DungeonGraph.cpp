#include "ds/DungeonGraph.h"
#include <iostream>

DungeonGraph::DungeonGraph() : roomCount(0) {
    for (int i = 0; i < MAX_ROOMS; ++i) {
        rooms[i] = nullptr;
        for (int d = 0; d < DIRECTION_COUNT; ++d)
            adjacency[i][d] = -1;
    }
}

DungeonGraph::~DungeonGraph() {
    for (int i = 0; i < roomCount; ++i) {
        delete rooms[i];
        rooms[i] = nullptr;
    }
}

bool DungeonGraph::isValidRoomId(int roomId) const {
    return roomId >= 0 && roomId < roomCount && rooms[roomId] != nullptr;
}

int DungeonGraph::addRoom(const std::string& name, const std::string& description) {
    if (roomCount >= MAX_ROOMS) return -1;
    int newId       = roomCount;
    rooms[newId]    = new Room(newId, name, description);
    ++roomCount;
    return newId;
}

bool DungeonGraph::connectRooms(int fromRoomId, Direction direction,
                                int toRoomId, bool bidirectional) {
    if (!isValidRoomId(fromRoomId) || !isValidRoomId(toRoomId)) return false;

    int dirIndex = directionToIndex(direction);
    if (dirIndex < 0) return false;

    adjacency[fromRoomId][dirIndex] = toRoomId;

    if (bidirectional) {
        int oppIndex = directionToIndex(oppositeDirection(direction));
        if (oppIndex >= 0)
            adjacency[toRoomId][oppIndex] = fromRoomId;
    }
    return true;
}

int DungeonGraph::getNeighbor(int fromRoomId, Direction direction) const {
    if (!isValidRoomId(fromRoomId)) return -1;
    int dirIndex = directionToIndex(direction);
    if (dirIndex < 0) return -1;
    return adjacency[fromRoomId][dirIndex];
}

Room* DungeonGraph::getRoom(int roomId) {
    if (!isValidRoomId(roomId)) return nullptr;
    return rooms[roomId];
}

const Room* DungeonGraph::getRoom(int roomId) const {
    if (!isValidRoomId(roomId)) return nullptr;
    return rooms[roomId];
}

int DungeonGraph::size() const { return roomCount; }

void DungeonGraph::printMap() const {
    std::cout << "\n=== 당근마켓 지도 ===\n";
    for (int i = 0; i < roomCount; ++i) {
        if (rooms[i] == nullptr) continue;
        std::cout << "  " << rooms[i]->getName() << ":";
        bool hasExit = false;
        for (int d = 0; d < DIRECTION_COUNT; ++d) {
            int neighbor = adjacency[i][d];
            if (neighbor != -1 && rooms[neighbor] != nullptr) {
                Direction dir = static_cast<Direction>(d);
                std::cout << " [" << directionToString(dir)
                          << "] -> " << rooms[neighbor]->getName();
                hasExit = true;
            }
        }
        if (!hasExit) std::cout << " (출구 없음)";
        std::cout << "\n";
    }
}
