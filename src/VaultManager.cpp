#include "VaultManager.h"

VaultManager::VaultManager(const std::filesystem::path& path)
    : vault_path(path) {
    db_path = vault_path / "vault.db";
}

sqlite3* VaultManager::db() {
    return database->get();
}
