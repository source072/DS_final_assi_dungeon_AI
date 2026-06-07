#ifndef GAME_H
#define GAME_H

#include "GameEvent.h"
#include "Jinsang.h"
#include "Player.h"
#include "ds/DungeonGraph.h"
#include "ds/Queue.h"
#include "ds/ScoreTree.h"
#include "ds/Stack.h"
#include <string>

class Game {
private:
    DungeonGraph      dungeon;
    Player            player;
    Stack<int>        moveHistory;
    Queue<GameEvent>  eventQueue;
    ScoreTree         scoreTree;
    bool              running;
    int               currentStage;    // 0~4 (0 = 아직 시작 전)
    int               minBet;          // 카지노 최소 베팅
    bool              stageCleared[4]; // 각 스테이지 클리어 여부
    int               enemyCount;      // 처치한 진상 수
    int               laborCount;      // 상하차 횟수
    Jinsang           jinsangs[4];     // 진상 4명

    // 방 ID 상수
    int roomHome;
    int roomMeeting[4]; // 약속 장소 1~4
    int roomCasino;
    int roomShop;
    int roomJail;

    void buildWorld();
    void buildJinsangs();
    void seedScores();

    // 커맨드 핸들러
    void printHelp()    const;
    void look()         const;
    void doMove(Direction direction);
    void undoMove();
    void doShop();
    void doCasino();
    void doBattle(int stageIndex);
    void doStatus()     const;
    void doInventory()  const;
    void doMap()        const;
    void doSortItems()  const;
    void doSell(const std::string& itemName);
    void triggerRandomEvent();

    // 전투 내부 헬퍼
    int  calcInitialProb(int jinsangPower) const;
    bool battleTurn(int& prob, const Jinsang& jinsang);

    // 카지노 내부 헬퍼
    void runSlotMachine();

    // 상점 NPC 멘트
    void printShopGreeting()              const;
    void printShopPurchaseMsg(int price)  const;
    void printShopNoMoneyMsg()            const;

    // 스테이지 해금 체크
    bool canUnlockStage(int stage) const;
    void onStageClear(int stage);

    void processCommand(const std::string& line);

public:
    Game();
    void run();
};

#endif
