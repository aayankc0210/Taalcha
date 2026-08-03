#include "db.h"
#include <iostream>

Database::Database(const std::string& path) : db(nullptr) {
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << path << "\n";
        db = nullptr;
    }
}

Database::~Database() {
    if (db)
        sqlite3_close(db);
}

bool Database::is_open() const {
    return db != nullptr;
}

sqlite3* Database::get() const {
    return db;
}

bool Database::execute(const std::string& sql) {
    char* err = nullptr;
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "SQL error: " << err << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}
