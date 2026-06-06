#include "Game.h"
#include "ds/Sorting.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <ctime>

// ─────────────────────────────────────────
//  생성자
// ─────────────────────────────────────────
Game::Game()
    : dungeon(), player("백춘식"),
      moveHistory(), eventQueue(), scoreTree(),
      running(true), currentStage(0), minBet(1000) {
    for (int i = 0; i < 4; ++i) stageCleared[i] = false;
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    buildWorld();
    buildJinsangs();
    seedScores();
}

// ─────────────────────────────────────────
//  세계 구축
// ─────────────────────────────────────────
void Game::buildWorld() {
    roomHome       = dungeon.addRoom("🏠 집",
        "반지하 원룸. 퀴퀴한 냄새가 나지만 그래도 내 집이다.\n"
        "여기서 쉬면서 전략을 짤 수 있다.");

    roomMeeting[0] = dungeon.addRoom("🤝 약속 장소 1",
        "동네 공원 벤치. 동네 진상이 기다리고 있다.");
    roomMeeting[1] = dungeon.addRoom("🤝 약속 장소 2",
        "카페 앞. 중고나라 진상이 아메리카노를 홀짝이며 기다린다.");
    roomMeeting[2] = dungeon.addRoom("🤝 약속 장소 3",
        "편의점 앞. 당근 진상이 팔짱을 끼고 서 있다.");
    roomMeeting[3] = dungeon.addRoom("🤝 약속 장소 4",
        "강남역 출구. 리셀계의 황제가 명품 백을 들고 서 있다.");

    roomCasino     = dungeon.addRoom("🎰 카지노",
        "번쩍번쩍한 슬롯머신들. 여기서 한 방을 노릴 수 있다.\n"
        "단, 잃을 수도 있다는 걸 명심할 것.");

    roomShop       = dungeon.addRoom("🏪 상점",
        "무기, 음식, 펫, 상품을 판매한다.\n"
        "'buy <아이템명>'으로 구매할 수 있다.");

    roomJail       = dungeon.addRoom("🔒 감옥",
        "...돈을 다 잃었다. 게임 오버.");

    // 연결: 집을 허브로
    dungeon.connectRooms(roomHome, Direction::North, roomMeeting[0], false);
    dungeon.connectRooms(roomMeeting[0], Direction::South, roomHome, false);

    dungeon.connectRooms(roomHome, Direction::East,  roomCasino, true);
    dungeon.connectRooms(roomHome, Direction::West,  roomShop,   true);

    // 약속 장소들은 집에서만 접근
    // (Stage 해금 시 연결 추가)

    // 상점 아이템 세팅
    Room* shop = dungeon.getRoom(roomShop);
    if (shop) {
        shop->addItem(Item("벽돌",      "단단한 벽돌. 기본 무기.",            10000, 5,  ItemType::Weapon));
        shop->addItem(Item("창",        "날카로운 창.",                         30000, 12, ItemType::Weapon));
        shop->addItem(Item("총",        "현대 무기의 정수.",                    80000, 25, ItemType::Weapon));
        shop->addItem(Item("엑스칼리버","전설의 검. 이걸 들면 무적이다.",      300000, 50, ItemType::Weapon));
        shop->addItem(Item("빵",        "편의점 빵. 마음의 위안.",               3000,  8, ItemType::Food));
        shop->addItem(Item("라면",      "뜨거운 라면 한 그릇.",                  5000, 12, ItemType::Food));
        shop->addItem(Item("햄버거",    "더블 패티 햄버거. 힘이 솟는다.",        8000, 15, ItemType::Food));
        shop->addItem(Item("강아지",    "귀여운 강아지. 카지노 꽝 확률 -5%.",  50000, 25, ItemType::Pet));
        shop->addItem(Item("고양이",    "쿨한 고양이. 카지노 꽝 확률 -5%.",   100000, 25, ItemType::Pet));
        shop->addItem(Item("컴퓨터",    "리셀용 컴퓨터. 물건 자랑 가능.",      80000, 15, ItemType::Goods));
        shop->addItem(Item("자동차",    "리셀용 자동차. 물건 자랑 가능.",      200000, 20, ItemType::Goods));
        shop->addItem(Item("빌딩",      "리셀용 빌딩. 물건 자랑 가능.",        500000, 30, ItemType::Goods));
    }

    // 초기 이벤트 큐
    eventQueue.enqueue(GameEvent("이웃집 할머니가 용돈을 주셨다.", 0, 5000));
    eventQueue.enqueue(GameEvent("길에서 천원을 주웠다!", 0, 1000));
    eventQueue.enqueue(GameEvent("갑자기 택배비가 나갔다...", 0, -3000));
}

// ─────────────────────────────────────────
//  진상 4명 세팅
// ─────────────────────────────────────────
void Game::buildJinsangs() {
    {
        JinsangSkill s[] = { JinsangSkill::Eojji };
        jinsangs[0] = Jinsang("동네 진상", 15, 1, s, 1);
    }
    {
        JinsangSkill s[] = { JinsangSkill::Eojji, JinsangSkill::Hwanbul };
        jinsangs[1] = Jinsang("중고나라 진상", 25, 2, s, 2);
    }
    {
        JinsangSkill s[] = { JinsangSkill::Nego, JinsangSkill::Hwanbul };
        jinsangs[2] = Jinsang("당근 진상", 38, 3, s, 2);
    }
    {
        JinsangSkill s[] = { JinsangSkill::Dokjeom, JinsangSkill::Nego, JinsangSkill::Hwanbul };
        jinsangs[3] = Jinsang("👑 리셀계의 황제", 55, 4, s, 3);
    }
}

void Game::seedScores() {
    scoreTree.insert(ScoreRecord("레전드 춘식", 1500000));
    scoreTree.insert(ScoreRecord("중수 춘식",    500000));
    scoreTree.insert(ScoreRecord("초보 춘식",    100000));
}

// ─────────────────────────────────────────
//  초기 확률 계산
// ─────────────────────────────────────────
int Game::calcInitialProb(int jinsangPower) const {
    int prob = 30 + (player.getCombatPower() - jinsangPower) * 2;
    if (prob < 0)   prob = 0;
    // 상한은 없음 (100 초과 시 무조건 성공)
    return prob;
}

// ─────────────────────────────────────────
//  전투 1턴 처리 (플레이어 액션 선택)
//  반환값: true = 거래 시도, false = 계속
// ─────────────────────────────────────────
bool Game::battleTurn(int& prob, const Jinsang& jinsang) {
    std::cout << "\n현재 거래 확률: " << prob << "%\n";
    std::cout << "행동을 선택하세요:\n";
    std::cout << "  1. 😤 설득하기       (확률 +10%)\n";
    std::cout << "  2. 💰 가격 할인      (보유 금액 10% 소모, 확률 +20%)\n";
    std::cout << "  3. 📸 물건 자랑      (상품 필요, 확률 +15%)\n";
    std::cout << "  4. 🐾 펫 등장        (펫 필요, 1회만, 확률 +25%)\n";
    std::cout << "  5. 🍔 음식 대접      (음식 소모, 확률 상승)\n";
    std::cout << "  6. 🤝 거래 시도      (현재 확률로 판정)\n";

    std::string input;
    std::cout << "> ";
    std::getline(std::cin, input);

    if (input == "1") {
        prob += 10;
        std::cout << "춘식: \"한번만 봐주세요...\" 설득 성공! 확률 +10%\n";

    } else if (input == "2") {
        int cost = player.getMoney() / 10;
        if (cost == 0) {
            std::cout << "돈이 너무 없어서 할인을 줄 수 없다...\n";
            return false;
        }
        player.spendMoney(cost);
        prob += 20;
        std::cout << cost << "원을 할인해줬다. 확률 +20%\n";
        if (!player.isAlive()) {
            std::cout << "\n💸 돈이 0원이 됐다! 감옥으로...\n";
            running = false;
            return true;
        }

    } else if (input == "3") {
        if (!player.getInventory().hasItemOfType(ItemType::Goods)) {
            std::cout << "자랑할 상품이 없다. 상점에서 상품을 구매하자.\n";
            return false;
        }
        prob += 15;
        std::cout << "상품을 꺼내 보여줬다. 진상의 눈이 번뜩인다. 확률 +15%\n";

    } else if (input == "4") {
        if (!player.getInventory().hasItemOfType(ItemType::Pet)) {
            std::cout << "펫이 없다.\n";
            return false;
        }
        // 펫은 1회만 사용 가능 (제거)
        Item* pet = nullptr;
        // 강아지 또는 고양이 찾기
        pet = player.getInventory().findItem("강아지");
        if (!pet) pet = player.getInventory().findItem("고양이");
        if (pet) {
            int bonus = pet->getEffectValue();
            std::string petName = pet->getName();
            player.getInventory().removeItem(petName);
            prob += bonus;
            std::cout << petName << "를 꺼냈다! 진상이 순간 녹아내린다. 확률 +"
                      << bonus << "%\n";
        }

    } else if (input == "5") {
        // 음식 찾기
        Item* food = player.getInventory().findItem("햄버거");
        if (!food) food = player.getInventory().findItem("라면");
        if (!food) food = player.getInventory().findItem("빵");
        if (!food) {
            std::cout << "음식이 없다.\n";
            return false;
        }
        int bonus = food->getEffectValue();
        std::string foodName = food->getName();
        player.getInventory().removeItem(foodName);
        prob += bonus;
        std::cout << foodName << "을 대접했다. 진상도 인간이었나... 확률 +"
                  << bonus << "%\n";

    } else if (input == "6") {
        // 거래 시도
        if (prob <= 0) {
            std::cout << "\n확률이 0%! 거래가 즉시 실패했다...\n";
            return true;
        }
        return true; // 판정은 호출부에서
    } else {
        std::cout << "1~6 중 하나를 입력하세요.\n";
        return false;
    }

    // 진상 턴: 방해
    SkillEffect effect = jinsang.useRandomSkill();
    std::cout << "\n진상의 방해: " << effect.message << "\n";
    prob += effect.probDelta;

    if (effect.moneySteal > 0) {
        int stolen = effect.moneySteal;
        if (stolen > player.getMoney()) stolen = player.getMoney();
        player.spendMoney(stolen);
        std::cout << stolen << "원을 빼앗겼다! 남은 돈: " << player.getMoney() << "원\n";
        if (!player.isAlive()) {
            std::cout << "\n💸 돈이 0원이 됐다! 감옥으로...\n";
            running = false;
            return true;
        }
    }

    if (prob <= 0) {
        std::cout << "\n확률이 0% 이하로 떨어졌다! 거래 즉시 실패.\n";
        return true;
    }

    return false;
}

// ─────────────────────────────────────────
//  전투 메인
// ─────────────────────────────────────────
void Game::doBattle(int stageIndex) {
    if (stageIndex < 0 || stageIndex >= 4) return;
    if (stageCleared[stageIndex]) {
        std::cout << "이미 클리어한 스테이지다.\n";
        return;
    }

    const Jinsang& jinsang = jinsangs[stageIndex];
    int prob = calcInitialProb(jinsang.getCombatPower());

    std::cout << "\n==============================\n";
    std::cout << "  거래 시작: " << jinsang.getName() << "\n";
    std::cout << "  진상 전투력: " << jinsang.getCombatPower() << "\n";
    std::cout << "  초기 거래 확률: " << prob << "%\n";
    std::cout << "==============================\n";

    bool tradeAttempted = false;
    while (running) {
        tradeAttempted = battleTurn(prob, jinsang);
        if (!running) return; // 돈 0원 감옥
        if (tradeAttempted) break;
    }

    // 거래 판정
    if (prob <= 0) {
        std::cout << "\n❌ 거래 실패! 다음에 다시 도전하자.\n";
        return;
    }
    if (prob >= 100) {
        std::cout << "\n✅ 확률 " << prob << "%! 거래 성공!\n";
        onStageClear(stageIndex);
        return;
    }

    int roll = std::rand() % 100 + 1;
    std::cout << "\n🎲 주사위: " << roll << " (필요: " << prob << " 이하)\n";
    if (roll <= prob) {
        std::cout << "✅ 거래 성공!\n";
        onStageClear(stageIndex);
    } else {
        std::cout << "❌ 거래 실패! 다음에 다시 도전하자.\n";
    }
}

// ─────────────────────────────────────────
//  스테이지 클리어 처리
// ─────────────────────────────────────────
bool Game::canUnlockStage(int stage) const {
    switch (stage) {
        case 0: // Stage 1: 돈 30,000원 + 상품 1개
            return player.getMoney() >= 30000
                && player.getInventory().hasItemOfType(ItemType::Goods);
        case 1: // Stage 2: 돈 100,000원 + 상품 보유
            return player.getMoney() >= 100000
                && player.getInventory().hasItemOfType(ItemType::Goods);
        case 2: // Stage 3: 돈 300,000원 + 무기
            return player.getMoney() >= 300000
                && player.getInventory().hasItemOfType(ItemType::Weapon);
        case 3: // Stage 4: 돈 1,000,000원 + 이전 스테이지 전부 클리어
            return player.getMoney() >= 1000000
                && stageCleared[0] && stageCleared[1] && stageCleared[2];
        default:
            return false;
    }
}

void Game::onStageClear(int stageIndex) {
    stageCleared[stageIndex] = true;
    currentStage = stageIndex + 1;

    // 카지노 최소 베팅 상승
    int bets[] = { 1000, 5000, 15000, 50000 };
    minBet = bets[currentStage < 4 ? currentStage : 3];

    // 약속 장소 다음 스테이지 연결
    if (currentStage < 4) {
        dungeon.connectRooms(roomHome, Direction::North, roomMeeting[currentStage], false);
        dungeon.connectRooms(roomMeeting[currentStage], Direction::South, roomHome, false);
    }

    // 보상
    int rewards[] = { 20000, 60000, 150000, 500000 };
    int reward = rewards[stageIndex];
    player.addMoney(reward);
    std::cout << "\n🎉 Stage " << (stageIndex + 1) << " 클리어! 보상: "
              << reward << "원\n";

    // 최종 클리어
    if (stageIndex == 3) {
        std::cout << "\n";
        std::cout << "══════════════════════════════════════\n";
        std::cout << "           🏆 GAME CLEAR 🏆            \n";
        std::cout << "══════════════════════════════════════\n";
        std::cout << "\n백춘식은 해냈다.\n";
        std::cout << "진상들을 모두 물리치고,\n";
        std::cout << "카지노에서 쪽박을 찰 뻔 했으면서도,\n";
        std::cout << "당근마켓에서 떼돈을 벌었다.\n\n";
        std::cout << "월세?  납부 완료.\n";
        std::cout << "카드값? 납부 완료.\n";
        std::cout << "삼각김밥? 이제 3개씩 산다.\n\n";
        std::cout << "그리고 춘식은 깨달았다.\n";
        std::cout << "\"세상은 결국 당근이다.\"\n\n";
        std::cout << "  최종 보유 금액: " << player.getMoney() << "원\n";
        std::cout << "  최종 전투력   : " << player.getCombatPower() << "\n";
        std::cout << "══════════════════════════════════════\n";
        scoreTree.insert(ScoreRecord(player.getName(), player.getMoney()));
        scoreTree.printDescending();
        running = false;
    }
}

// ─────────────────────────────────────────
//  카지노 슬롯머신
// ─────────────────────────────────────────
void Game::runSlotMachine() {
    std::cout << "\n=== 🎰 슬롯머신 ===\n";
    std::cout << "현재 최소 베팅: " << minBet << "원\n";
    std::cout << "보유 금액: " << player.getMoney() << "원\n";
    std::cout << "베팅 금액 입력 (0 = 취소): ";

    std::string input;
    std::getline(std::cin, input);
    int bet = 0;
    try { bet = std::stoi(input); } catch (...) { bet = 0; }

    if (bet == 0) { std::cout << "카지노를 나왔다.\n"; return; }
    if (bet < minBet) {
        std::cout << "최소 베팅 금액은 " << minBet << "원입니다.\n";
        return;
    }
    if (!player.spendMoney(bet)) {
        std::cout << "돈이 부족하다!\n";
        return;
    }

    // 펫 보유 시 꽝 확률 -5%
    bool hasPet = player.getInventory().hasItemOfType(ItemType::Pet);
    // 확률: 잭팟 5%, 대박 20%, 소박 35%, 꽝 40% (펫 있으면 꽝 35%)
    int roll = std::rand() % 100;
    int jackpotThres = 5;
    int bigThres     = 25;   // 5+20
    int smallThres   = 60;   // 5+20+35
    // 꽝 확률: 나머지 (40% or 35%)

    std::cout << "\n🎰 슬롯 결과: ";
    if (roll < jackpotThres) {
        int win = bet * 5;
        player.addMoney(win);
        std::cout << "💎💎💎 JACKPOT! +" << win << "원!\n";
    } else if (roll < bigThres) {
        int win = bet * 2;
        player.addMoney(win);
        std::cout << "🍋🍋🍋 대박! +" << win << "원!\n";
    } else if (roll < smallThres) {
        int win = static_cast<int>(bet * 1.5);
        player.addMoney(win);
        std::cout << "🍒🍒🍒 소박! +" << win << "원!\n";
    } else {
        int bangThres = hasPet ? 95 : 100; // 펫 있으면 꽝 확률 -5%
        if (roll < bangThres || !hasPet) {
            std::cout << "💀 꽝... -" << bet << "원\n";
            if (!player.isAlive()) {
                std::cout << "\n💸 돈이 0원이 됐다! 감옥으로...\n";
                player.setCurrentRoomId(roomJail);
                dungeon.getRoom(roomJail)->printDescription();
                running = false;
            }
        } else {
            // 펫 덕분에 아슬아슬 살아남
            int win = bet; // 본전
            player.addMoney(win);
            std::cout << "🐾 펫 덕분에 겨우 본전! +" << win << "원\n";
        }
    }
    std::cout << "남은 돈: " << player.getMoney() << "원\n";
}

// ─────────────────────────────────────────
//  상점
// ─────────────────────────────────────────
void Game::doShop() {
    Room* shop = dungeon.getRoom(roomShop);
    if (!shop) return;

    std::cout << "\n=== 🏪 상점 ===\n";
    std::cout << "보유 금액: " << player.getMoney() << "원\n";
    std::cout << "구매: buy <아이템명>  |  목록: look\n";

    int count = shop->itemCount();
    if (count == 0) { std::cout << "재고가 없습니다.\n"; return; }

    // 정렬 후 출력
    Item* copy = new Item[count];
    for (int i = 0; i < count; ++i) copy[i] = shop->getItem(i);
    sortItemsByValueDescending(copy, count);
    std::cout << "상품 목록:\n";
    for (int i = 0; i < count; ++i) {
        std::cout << "  - ";
        copy[i].print();
    }
    delete[] copy;
}

// ─────────────────────────────────────────
//  커맨드 핸들러들
// ─────────────────────────────────────────
void Game::printHelp() const {
    std::cout << "\n=== 도움말 ===\n";
    std::cout << "  help             도움말\n";
    std::cout << "  look             현재 장소 설명\n";
    std::cout << "  move <dir>       이동 (north/south/east/west)\n";
    std::cout << "  undo             이전 장소로 되돌아가기\n";
    std::cout << "  buy <아이템명>   상점에서 아이템 구매\n";
    std::cout << "  battle           현재 약속 장소에서 거래 시작\n";
    std::cout << "  casino           슬롯머신 (카지노에서만)\n";
    std::cout << "  inventory        인벤토리 확인\n";
    std::cout << "  status           상태 확인\n";
    std::cout << "  map              지도 확인\n";
    std::cout << "  scores           점수 랭킹\n";
    std::cout << "  event            이벤트 처리\n";
    std::cout << "  quit             게임 종료\n";
}

void Game::look() const {
    const Room* room = dungeon.getRoom(player.getCurrentRoomId());
    if (!room) { std::cout << "알 수 없는 장소.\n"; return; }
    room->printDescription();
    std::cout << "[이동 가능한 방향]";
    bool hasExit = false;
    for (int i = 0; i < 4; ++i) {
        Direction dir = static_cast<Direction>(i);
        int neighbor = dungeon.getNeighbor(room->getId(), dir);
        if (neighbor != -1) {
            const Room* nb = dungeon.getRoom(neighbor);
            if (nb) {
                std::cout << " " << directionToString(dir)
                          << "(" << nb->getName() << ")";
                hasExit = true;
            }
        }
    }
    if (!hasExit) std::cout << " 없음";
    std::cout << "\n";
}

void Game::doMove(Direction direction) {
    if (direction == Direction::Invalid) {
        std::cout << "올바른 방향을 입력하세요 (north/south/east/west).\n";
        return;
    }
    int current = player.getCurrentRoomId();
    int next    = dungeon.getNeighbor(current, direction);
    if (next == -1) {
        std::cout << "그 방향으로는 갈 수 없다.\n";
        return;
    }
    moveHistory.push(current);
    player.setCurrentRoomId(next);
    look();
}

void Game::undoMove() {
    int prev = -1;
    if (!moveHistory.pop(prev)) {
        std::cout << "되돌아갈 곳이 없다.\n";
        return;
    }
    player.setCurrentRoomId(prev);
    std::cout << "이전 장소로 돌아왔다.\n";
    look();
}

void Game::doInventory() const {
    player.getInventory().print();
}

void Game::doStatus() const {
    player.printStatus();
    std::cout << "  현재 스테이지: " << currentStage << "\n";
    std::cout << "  카지노 최소 베팅: " << minBet << "원\n";
}

void Game::doMap() const {
    dungeon.printMap();
}

void Game::doScores() const {
    scoreTree.printDescending();
}

void Game::doSortItems() const {
    const Room* room = dungeon.getRoom(player.getCurrentRoomId());
    if (!room || room->itemCount() == 0) {
        std::cout << "이 장소에 아이템이 없다.\n";
        return;
    }
    int count = room->itemCount();
    Item* copy = new Item[count];
    for (int i = 0; i < count; ++i) copy[i] = room->getItem(i);
    sortItemsByValueDescending(copy, count);
    std::cout << "[가격 높은 순 아이템]\n";
    for (int i = 0; i < count; ++i) {
        std::cout << "  - ";
        copy[i].print();
    }
    delete[] copy;
}

// ─────────────────────────────────────────
//  buy 커맨드
// ─────────────────────────────────────────
void Game::processCommand(const std::string& line) {
    std::istringstream ss(line);
    std::string cmd;
    ss >> cmd;

    if (cmd == "help") {
        printHelp();
    } else if (cmd == "look") {
        look();
    } else if (cmd == "move") {
        std::string dir;
        ss >> dir;
        doMove(parseDirection(dir));
    } else if (cmd == "undo") {
        undoMove();
    } else if (cmd == "buy") {
        std::string itemName;
        std::getline(ss, itemName);
        // 앞 공백 제거
        while (!itemName.empty() && itemName[0] == ' ') itemName.erase(0, 1);
        if (player.getCurrentRoomId() != roomShop) {
            std::cout << "상점에서만 구매할 수 있다.\n";
            return;
        }
        Room* shop = dungeon.getRoom(roomShop);
        if (!shop) return;
        Item item;
        if (!shop->takeItem(itemName, item)) {
            std::cout << "'" << itemName << "' 아이템을 찾을 수 없다.\n";
            return;
        }
        if (!player.spendMoney(item.getValue())) {
            std::cout << "돈이 부족하다! (필요: " << item.getValue()
                      << "원, 보유: " << player.getMoney() << "원)\n";
            shop->addItem(item); // 다시 넣기
            return;
        }
        // 무기/음식 효과 즉시 적용
        if (item.getType() == ItemType::Weapon) {
            player.addCombatPower(item.getEffectValue());
            std::cout << item.getName() << " 구매 완료! 전투력 +"
                      << item.getEffectValue() << " (현재: "
                      << player.getCombatPower() << ")\n";
        } else {
            player.getInventory().addItem(item);
            std::cout << item.getName() << " 구매 완료!\n";
        }
        if (!player.isAlive()) {
            std::cout << "💸 돈이 0원! 감옥으로...\n";
            running = false;
        }
    } else if (cmd == "battle") {
        // 현재 방이 약속 장소인지 확인
        int rid = player.getCurrentRoomId();
        int stageIdx = -1;
        for (int i = 0; i < 4; ++i) {
            if (rid == roomMeeting[i]) { stageIdx = i; break; }
        }
        if (stageIdx == -1) {
            std::cout << "약속 장소에서만 거래할 수 있다.\n";
            return;
        }
        if (!canUnlockStage(stageIdx)) {
            std::cout << "해금 조건을 만족하지 못했다.\n";
            switch (stageIdx) {
                case 0: std::cout << "  필요: 돈 30,000원 + 상품 1개\n"; break;
                case 1: std::cout << "  필요: 돈 100,000원 + 상품 2개\n"; break;
                case 2: std::cout << "  필요: 돈 300,000원 + 무기\n"; break;
                case 3: std::cout << "  필요: 돈 1,000,000원 + 이전 스테이지 전부 클리어\n"; break;
            }
            return;
        }
        doBattle(stageIdx);
    } else if (cmd == "casino") {
        if (player.getCurrentRoomId() != roomCasino) {
            std::cout << "카지노에서만 슬롯머신을 이용할 수 있다.\n";
            return;
        }
        runSlotMachine();
    } else if (cmd == "inventory") {
        doInventory();
    } else if (cmd == "status") {
        doStatus();
    } else if (cmd == "map") {
        doMap();
    } else if (cmd == "scores") {
        doScores();
    } else if (cmd == "sortitems") {
        doSortItems();
    } else if (cmd == "event") {
        GameEvent ev;
        if (!eventQueue.dequeue(ev)) {
            std::cout << "처리할 이벤트가 없다.\n";
            return;
        }
        std::cout << "📢 이벤트: " << ev.description << "\n";
        if (ev.moneyDelta != 0) {
            player.addMoney(ev.moneyDelta);
            std::cout << "  돈 변화: " << ev.moneyDelta << "원 (현재: "
                      << player.getMoney() << "원)\n";
        }
        if (!player.isAlive()) {
            std::cout << "💸 돈이 0원! 감옥으로...\n";
            running = false;
        }
    } else if (cmd == "quit") {
        running = false;
    } else if (cmd.empty()) {
        // 빈 줄 무시
    } else {
        std::cout << "알 수 없는 명령어. 'help'를 입력하세요.\n";
    }
}

// ─────────────────────────────────────────
//  게임 실행
// ─────────────────────────────────────────
void Game::run() {
    // 오프닝 나레이션
    std::cout << "\n";
    std::cout << "══════════════════════════════════════\n";
    std::cout << "        당근로운 평화마켓              \n";
    std::cout << "══════════════════════════════════════\n";
    std::cout << "\n때는 2025년. 대한민국 어딘가의 반지하 원룸.\n";
    std::cout << "23살 백수 백춘식은 오늘도 유튜브 쇼츠를 보며 하루를 시작했다.\n\n";
    std::cout << "\"월세가 밀렸다.\"\n";
    std::cout << "\"카드값이 밀렸다.\"\n";
    std::cout << "\"심지어 편의점 삼각김밥도 이제 2,000원이다.\"\n\n";
    std::cout << "그때, 핸드폰에 알림 하나가 떴다.\n";
    std::cout << "📱 [당근마켓] \"갤럭시 S23 10만원에 팝니다 진짜 급해요 ㅠㅠ\"\n\n";
    std::cout << "춘식의 눈이 번뜩였다. \"...리셀이다.\"\n\n";
    std::cout << "과연 춘식은 이 험난한 당근마켓에서\n";
    std::cout << "살아남아 월세를 낼 수 있을까?\n\n";
    std::cout << "[ 시작 금액: 50,000원 | 전투력: 10 ]\n";
    std::cout << "══════════════════════════════════════\n";
    std::cout << "'help'를 입력하면 명령어 목록을 볼 수 있습니다.\n\n";

    look();

    while (running && player.isAlive()) {
        std::cout << "\n[💰" << player.getMoney() << "원 | ⚔️"
                  << player.getCombatPower() << "] > ";
        std::string line;
        if (!std::getline(std::cin, line)) break;
        processCommand(line);
    }

    if (!player.isAlive()) {
        std::cout << "\n══════════════════════════════════════\n";
        std::cout << "           💀 GAME OVER               \n";
        std::cout << "══════════════════════════════════════\n";
        std::cout << "돈이 0원이 됐다. 춘식은 결국 감옥에 끌려갔다.\n";
        std::cout << "다음 생엔 제발 저축 좀 하자...\n";
    }

    scoreTree.insert(ScoreRecord(player.getName(), player.getMoney()));
    std::cout << "\n최종 보유 금액: " << player.getMoney() << "원\n";
    scoreTree.printDescending();
    std::cout << "Goodbye.\n";
}
