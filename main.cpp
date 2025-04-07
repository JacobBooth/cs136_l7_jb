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

void loadRecordsFromFile() {
    ifstream input(INPUT_FILE);
    if (!input) {
        cout << "Cannot open input file.\n";
        return;
    }

    Student temp;
    while (input >> temp.id >> temp.firstName >> temp.lastName >> temp.units) {
        temp.archived = false;
        temp.occupied = true;
        insertRecord(temp);
    }
    input.close();

    ofstream out(UNPROCESSED_FILE);
    for (int i = 0; i < unprocessedCount; ++i) {
        out << unprocessedTable[i].id << ' '
            << unprocessedTable[i].firstName << ' '
            << unprocessedTable[i].lastName << ' '
            << unprocessedTable[i].units << "\n";
    }
    out.close();
}

void searchById(const char id[]) {
    int index = extractHashIndex(id, hashTableSize);
    if (hashTable[index].occupied && strcmp(hashTable[index].id, id) == 0) {
        cout << "Found in hash table: " << hashTable[index].firstName << ' ' << hashTable[index].lastName << "\n";
        return;
    }
    for (int i = 0; i < overflowSize; ++i) {
        if (overflowTable[i].occupied && strcmp(overflowTable[i].id, id) == 0) {
            cout << "Found in overflow table: " << overflowTable[i].firstName << ' ' << overflowTable[i].lastName << "\n";
            return;
        }
    }
    cout << "Record not found.\n";
}

void archiveById(const char id[]) {
    int index = extractHashIndex(id, hashTableSize);
    if (hashTable[index].occupied && strcmp(hashTable[index].id, id) == 0) {
        hashTable[index].archived = true;
        cout << "Archived.\n";
        return;
    }
    for (int i = 0; i < overflowSize; ++i) {
        if (overflowTable[i].occupied && strcmp(overflowTable[i].id, id) == 0) {
            overflowTable[i].archived = true;
            cout << "Archived.\n";
            return;
        }
    }
    cout << "Record not found.\n";
}

void printRecords(bool archived) {
    cout << left << setw(12) << "ID" << setw(20) << "First Name" << setw(20) << "Last Name" << "Units\n";
    for (int i = 0; i < hashTableSize; ++i) {
        if (hashTable[i].occupied && hashTable[i].archived == archived) {
            cout << setw(12) << hashTable[i].id << setw(20) << hashTable[i].firstName << setw(20) << hashTable[i].lastName << hashTable[i].units << "\n";
        }
    }
    for (int i = 0; i < overflowSize; ++i) {
        if (overflowTable[i].occupied && overflowTable[i].archived == archived) {
            cout << setw(12) << overflowTable[i].id << setw(20) << overflowTable[i].firstName << setw(20) << overflowTable[i].lastName << overflowTable[i].units << "\n";
        }
    }
}

void printUnprocessed() {
    ifstream in(UNPROCESSED_FILE);
    string line;
    while (getline(in, line)) {
        cout << line << "\n";
    }
    in.close();
}

int main() {
    int choice = 0;
    while (choice != QUIT) {
        cout << "\n\nMenu:\n"
             << "1. Create Hash and Overflow Tables\n"
             << "2. Search for a record by ID\n"
             << "3. Archive a record\n"
             << "4. Print Active Records\n"
             << "5. Print Archived Records\n"
             << "6. Print Unprocessed Records\n"
             << "7. Quit\n"
             << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear(); cin.ignore(1000, '\n');
            continue;
        }

        switch (choice) {
            case CREATE:
                cout << "Enter hash table size (10, 100, 1000): ";
                cin >> hashTableSize;
                overflowSize = hashTableSize * 0.2;
                loadRecordsFromFile();
                cout << "Tables loaded.\n";
                break;
            case SEARCH: {
                char id[ID_LENGTH];
                cout << "Enter ID: ";
                cin >> id;
                searchById(id);
                break;
            }
            case ARCHIVE: {
                char id[ID_LENGTH];
                cout << "Enter ID to archive: ";
                cin >> id;
                archiveById(id);
                break;
            }
            case PRINT_ACTIVE:
                printRecords(false);
                break;
            case PRINT_ARCHIVED:
                printRecords(true);
                break;
            case PRINT_UNPROCESSED:
                printUnprocessed();
                break;
            case QUIT:
                cout << "Goodbye!\n";
                break;
        }
    }
    return 0;

    /*
    Menu:
1. Create Hash and Overflow Tables
2. Search for a record by ID
3. Archive a record
4. Print Active Records
5. Print Archived Records
6. Print Unprocessed Records
7. Quit
Enter choice: 1
Enter hash table size (10, 100, 1000): 10
Tables loaded.


Menu:
1. Create Hash and Overflow Tables
2. Search for a record by ID
3. Archive a record
4. Print Active Records
5. Print Archived Records
6. Print Unprocessed Records
7. Quit
Enter choice: 6
881234572 Mark Blue 22
991234561 Nancy Gray 27
881234573 Peter White 31
881234574 Rachel Black 15
991234562 Steve Purple 17


Menu:
1. Create Hash and Overflow Tables
2. Search for a record by ID
3. Archive a record
4. Print Active Records
5. Print Archived Records
6. Print Unprocessed Records
7. Quit
Enter choice: 5
ID          First Name          Last Name           Units


Menu:
1. Create Hash and Overflow Tables
2. Search for a record by ID
3. Archive a record
4. Print Active Records
5. Print Archived Records
6. Print Unprocessed Records
7. Quit
Enter choice: 4
ID          First Name          Last Name           Units
1           Mary                Marcos              25
991234570   Bob                 Brown               40
991234560   John                Smith               32
881234580   Alice               Johnson             18
881234571   Linda               Green               12


Menu:
1. Create Hash and Overflow Tables
2. Search for a record by ID
3. Archive a record
4. Print Active Records
5. Print Archived Records
6. Print Unprocessed Records
7. Quit
Enter choice: 7
Goodbye!
    */