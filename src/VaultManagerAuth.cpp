#include "VaultManager.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <openssl/rand.h>

bool VaultManager::setup(const std::string& password) {
    // Generate a random 16-byte salt, store it as hex
    uint8_t salt_bytes[16];
    RAND_bytes(salt_bytes, 16);

    std::ostringstream ss;
    for (int i = 0; i < 16; i++)
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)salt_bytes[i];
    std::string salt_hex = ss.str();

    // Store hash of (password + salt) for verification
    // We re-use hash_password but pass password+salt combined
    std::string combined = password + salt_hex;
    std::string hash = hash_password(combined);
    if (hash.empty()) return false;

    sqlite3_stmt* stmt;
    const char* sql = "INSERT OR REPLACE INTO vault_meta (key, value) VALUES (?, ?);";

    auto ins = [&](const char* key, const std::string& val) -> bool {
        if (sqlite3_prepare_v2(db(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
        sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, val.c_str(), -1, SQLITE_TRANSIENT);
        bool ok = sqlite3_step(stmt) == SQLITE_DONE;
        sqlite3_finalize(stmt);
        return ok;
    };

    if (!ins("master_hash", hash)) return false;
    if (!ins("master_salt", salt_hex)) return false;

    if (!derive_key(password, salt_hex)) return false;
    unlocked = true;
    return true;
}

bool VaultManager::unlock(const std::string& password) {
    sqlite3_stmt* stmt;

    auto get_meta = [&](const char* key) -> std::string {
        const char* sql = "SELECT value FROM vault_meta WHERE key=?;";
        if (sqlite3_prepare_v2(db(), sql, -1, &stmt, nullptr) != SQLITE_OK) return "";
        sqlite3_bind_text(stmt, 1, key, -1, SQLITE_STATIC);
        std::string val;
        if (sqlite3_step(stmt) == SQLITE_ROW)
            val = (const char*)sqlite3_column_text(stmt, 0);
        sqlite3_finalize(stmt);
        return val;
    };

    std::string stored_hash = get_meta("master_hash");
    std::string salt_hex    = get_meta("master_salt");

    if (stored_hash.empty() || salt_hex.empty()) return false;

    // Verify: hash(password + salt) should match stored hash
    std::string combined = password + salt_hex;
    if (!verify_password(combined, stored_hash)) return false;

    if (!derive_key(password, salt_hex)) return false;
    unlocked = true;
    return true;
}

void VaultManager::lock() {
    master_key.clear();
    unlocked = false;
}

bool VaultManager::is_unlocked() const { return unlocked; }

bool VaultManager::is_initialized() {
    if (!db()) return false;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT value FROM vault_meta WHERE key='master_hash';";
    if (sqlite3_prepare_v2(db(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    bool ok = (sqlite3_step(stmt) == SQLITE_ROW);
    sqlite3_finalize(stmt);
    return ok;
}
