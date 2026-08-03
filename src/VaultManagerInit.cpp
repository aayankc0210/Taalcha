#include "VaultManager.h"
#include <iostream>

bool VaultManager::create_schema() {
    const std::string sql = R"(
    CREATE TABLE IF NOT EXISTS vault_meta (
        key TEXT PRIMARY KEY,
        value TEXT NOT NULL
    );

    CREATE TABLE IF NOT EXISTS entries (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        type TEXT NOT NULL,
        title TEXT NOT NULL,
        username TEXT,
        password_encrypted TEXT,
        note_encrypted TEXT,
        created_at INTEGER NOT NULL,
        updated_at INTEGER NOT NULL
    );

    CREATE TABLE IF NOT EXISTS entry_urls (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        entry_id INTEGER NOT NULL,
        url TEXT NOT NULL,
        FOREIGN KEY(entry_id) REFERENCES entries(id)
    );
    )";

    return database->execute(sql);
}

bool VaultManager::init() {
    std::filesystem::create_directories(vault_path);
    database = std::make_unique<Database>(db_path.string());
    if (!database->is_open()) return false;
    return create_schema();
}
