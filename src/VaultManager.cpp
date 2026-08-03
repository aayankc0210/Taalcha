#include "VaultManager.h"
using namespace std;

VaultManager::VaultManager(const filesystem::path& path)
    : vault_path(path) {
    db_path = vault_path / "vault.db";
}

sqlite3* VaultManager::db() {
    return database->get();
}