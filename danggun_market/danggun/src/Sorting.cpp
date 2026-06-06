#include "ds/Sorting.h"

// 선택 정렬: 아이템 가격 내림차순
void sortItemsByValueDescending(Item* items, int count) {
    for (int i = 0; i < count - 1; ++i) {
        int maxIdx = i;
        for (int j = i + 1; j < count; ++j) {
            if (items[j].getValue() > items[maxIdx].getValue())
                maxIdx = j;
        }
        if (maxIdx != i) {
            Item temp    = items[i];
            items[i]     = items[maxIdx];
            items[maxIdx] = temp;
        }
    }
}

// 삽입 정렬: 점수 내림차순
void sortScoresDescending(ScoreRecord* records, int count) {
    for (int i = 1; i < count; ++i) {
        ScoreRecord key = records[i];
        int j = i - 1;
        while (j >= 0 && records[j].score < key.score) {
            records[j + 1] = records[j];
            --j;
        }
        records[j + 1] = key;
    }
}
