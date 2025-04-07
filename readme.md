Objective

Learn to write and use hash tables by building a menu-driven student record system using shift-folding and mid-squared hashing techniques with collision handling via linear probing.

Program Features

Menu

Create Hash and Overflow Tables

Search for a Record by ID

Archive a Record

Print Active Records

Print Archived Records

Print Unprocessed Records

Quit

Details

Hash table size is user-defined (10, 100, or 1000)

Overflow table is 20% of hash table size

If both hash and overflow tables are full, remaining records go to unprocessed.txt

Archived records stay in their location but are flagged

Searching will state whether record was found and where (hash or overflow)

Notes

Test data should cover edge cases (full tables, collisions, etc.)

unprocessed.txt will be created if some records cannot be inserted

Use students.txt as your input file