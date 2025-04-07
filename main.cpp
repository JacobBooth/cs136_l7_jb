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

