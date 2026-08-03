#pragma once

#include "db.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

struct Entry {
    int id;
    std::string type;
    std::string title;
    std::string username;
    std::string password;
    std::string note;
    std::string url;
    std::string favicon_path;
};

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

    bool add_login(const std::string& title, const std::string& username,
                   const std::string& password,
                   const std::vector<std::string>& urls);
    bool add_note(const std::string& title, const std::string& note);

    bool update_login(int id, const std::string& username,
                      const std::string& password, const std::string& url);
    bool update_note(int id, const std::string& note);

    std::vector<Entry> get_entries();
    bool delete_entry(int id);

    std::filesystem::path get_vault_path() const { return vault_path; }
};
