#pragma once
#include "sqlite3.h"
#include <string>
using namespace std;

class Database {
private:
    sqlite3* db;

public:
    Database(const string& path);
    ~Database();

    bool is_open() const;
    sqlite3* get() const;
    bool execute(const string& sql);
};