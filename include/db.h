#pragma once
#include "sqlite3.h"
#include <string>

class Database {
private:
    sqlite3* db;

public:
    Database(const std::string& path);
    ~Database();

    bool is_open() const;
    sqlite3* get() const;
    bool execute(const std::string& sql);
};
