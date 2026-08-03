#pragma once

#include "db.h"
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
using namespace std;

struct Entry {
    int id;
    string type;
    string title;
    string username;
    string password;
    string note;
    string url;
    string favicon_path;
};

class VaultManager {
private:
    filesystem::path vault_path;
    filesystem::path db_path;

    unique_ptr<Database> database;
    bool unlocked = false;
    vector<uint8_t> master_key;

    sqlite3* db();

    bool create_schema();

    string encrypt(const string& data);
    string decrypt(const string& data);

    bool derive_key(const string& password, const string& salt_hex);
    string hash_password(const string& password);
    bool verify_password(const string& password, const string& stored_hash);

public:
    VaultManager(const filesystem::path& path);

    bool init();
    bool setup(const string& master_password);
    bool unlock(const string& master_password);
    void lock();
    bool is_initialized();
    bool is_unlocked() const;

    bool add_login(const string& title, const string& username,
                   const string& password,
                   const vector<string>& urls);
    bool add_note(const string& title, const string& note);

    bool update_login(int id, const string& username,
                      const string& password, const string& url);
    bool update_note(int id, const string& note);

    vector<Entry> get_entries();
    bool delete_entry(int id);

    filesystem::path get_vault_path() const { return vault_path; }
};