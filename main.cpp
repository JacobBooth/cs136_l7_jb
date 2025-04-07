#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>

using namespace std;

const int MAX_RECORDS = 1000;
const int ID_LENGTH = 10;
const int NAME_LENGTH = 20;
const char INPUT_FILE[] = "students.txt";
const char UNPROCESSED_FILE[] = "unprocessed.txt";

struct Student {
    char id[ID_LENGTH];
    char firstName[NAME_LENGTH];
    char lastName[NAME_LENGTH];
    int units;
    bool archived = false;
    bool occupied = false;
};

// Hash + overflow tables
Student hashTable[MAX_RECORDS] = {};
Student overflowTable[MAX_RECORDS] = {};
Student unprocessedTable[MAX_RECORDS] = {};

int hashTableSize = 0;
int overflowSize = 0;
int unprocessedCount = 0;

// Enum for menu
enum MenuOption {
    CREATE = 1,
    SEARCH,
    ARCHIVE,
    PRINT_ACTIVE,
    PRINT_ARCHIVED,
    PRINT_UNPROCESSED,
    QUIT
};

int extractHashIndex(const char id[], int tableSize) {
    char trimmed[8] = {};
    strncpy(trimmed, id + 2, 7);
    trimmed[7] = '\0';

    char group1[4] = {}, group2[5] = {};
    strncpy(group1, trimmed, 3);
    strncpy(group2, trimmed + 3, 4);

    for (int i = 0; i < 3 / 2; ++i) {
        swap(group1[i], group1[2 - i]);
    }
    for (int i = 0; i < 4 / 2; ++i) {
        swap(group2[i], group2[3 - i]);
    }

    int folded = atoi(group1) + atoi(group2);
    long long squared = (long long)folded * folded;

    char squaredStr[20];
    sprintf(squaredStr, "%lld", squared);
    int len = strlen(squaredStr);

    if (tableSize == 10) {
        return squaredStr[len / 2] - '0';
    } else if (tableSize == 100) {
        return ((squaredStr[len / 2] - '0') * 10 + (squaredStr[len / 2 + 1] - '0')) % tableSize;
    } else {
        return ((squaredStr[len / 2] - '0') * 100 + (squaredStr[len / 2 + 1] - '0') * 10 + (squaredStr[len / 2 + 2] - '0')) % tableSize;
    }
}

void insertRecord(const Student& s) {
    int index = extractHashIndex(s.id, hashTableSize);

    if (!hashTable[index].occupied) {
        hashTable[index] = s;
        hashTable[index].occupied = true;
    } else {
        bool placed = false;
        for (int i = 0; i < overflowSize; ++i) {
            if (!overflowTable[i].occupied) {
                overflowTable[i] = s;
                overflowTable[i].occupied = true;
                placed = true;
                break;
            }
        }
        if (!placed) {
            if (unprocessedCount < MAX_RECORDS) {
                unprocessedTable[unprocessedCount++] = s;
            }
        }
    }
}
