#pragma once

#include "db.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

class VaultManager {
private:
    std::filesystem::path vault_path;
    std::filesystem::path db_path;

    std::unique_ptr<Database> database;
    bool unlocked = false;
    std::vector<uint8_t> master_key;

    sqlite3* db();

    bool create_schema();

    std::string encrypt(const std::string& data);
    std::string decrypt(const std::string& data);

    bool derive_key(const std::string& password, const std::string& salt_hex);
    std::string hash_password(const std::string& password);
    bool verify_password(const std::string& password, const std::string& stored_hash);

public:
    VaultManager(const std::filesystem::path& path);

    bool init();
    bool setup(const std::string& master_password);
    bool unlock(const std::string& master_password);
    void lock();
    bool is_initialized();
    bool is_unlocked() const;

    std::filesystem::path get_vault_path() const { return vault_path; }
};
