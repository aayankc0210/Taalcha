#include "VaultManager.h"
#include <ctime>
#include <iostream>
using namespace std;

bool VaultManager::add_login(const string &title,
                              const string &username,
                              const string &password,
                              const vector<string> &urls) {
  if (!unlocked) return false;

  string enc_pass = encrypt(password);
  int now = (int)time(nullptr);

  sqlite3_stmt *stmt;
  const char *sql =
      "INSERT INTO entries (type, title, username, password_encrypted, created_at, updated_at)"
      " VALUES ('login', ?, ?, ?, ?, ?);";

  if (sqlite3_prepare_v2(db(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
  sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, enc_pass.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 4, now);
  sqlite3_bind_int(stmt, 5, now);

  if (sqlite3_step(stmt) != SQLITE_DONE) { sqlite3_finalize(stmt); return false; }
  sqlite3_finalize(stmt);

  int entry_id = (int)sqlite3_last_insert_rowid(db());
  const char *url_sql = "INSERT INTO entry_urls (entry_id, url) VALUES (?, ?);";
  for (const auto &u : urls) {
    if (u.empty()) continue;
    sqlite3_prepare_v2(db(), url_sql, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, entry_id);
    sqlite3_bind_text(stmt, 2, u.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }
  return true;
}

bool VaultManager::add_note(const string &title, const string &note) {
  if (!unlocked) return false;

  string enc_note = encrypt(note);
  int now = (int)time(nullptr);

  sqlite3_stmt *stmt;
  const char *sql =
      "INSERT INTO entries (type, title, note_encrypted, created_at, updated_at)"
      " VALUES ('note', ?, ?, ?, ?);";

  if (sqlite3_prepare_v2(db(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
  sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, enc_note.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 3, now);
  sqlite3_bind_int(stmt, 4, now);

  bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);
  return ok;
}

bool VaultManager::update_login(int id, const string &username,
                                 const string &password,
                                 const string &url) {
  if (!unlocked) return false;

  string enc_pass = encrypt(password);
  int now = (int)time(nullptr);

  sqlite3_stmt *stmt;
  const char *sql =
      "UPDATE entries SET username=?, password_encrypted=?, updated_at=? WHERE id=?;";

  if (sqlite3_prepare_v2(db(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
  sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, enc_pass.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 3, now);
  sqlite3_bind_int(stmt, 4, id);
  bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);

  if (!url.empty()) {
    // replace url: delete old, insert new
    const char *del = "DELETE FROM entry_urls WHERE entry_id=?;";
    sqlite3_prepare_v2(db(), del, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    const char *ins = "INSERT INTO entry_urls (entry_id, url) VALUES (?, ?);";
    sqlite3_prepare_v2(db(), ins, -1, &stmt, nullptr);
    sqlite3_bind_int(stmt, 1, id);
    sqlite3_bind_text(stmt, 2, url.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
  }

  return ok;
}

bool VaultManager::update_note(int id, const string &note) {
  if (!unlocked) return false;

  string enc_note = encrypt(note);
  int now = (int)time(nullptr);

  sqlite3_stmt *stmt;
  const char *sql = "UPDATE entries SET note_encrypted=?, updated_at=? WHERE id=?;";
  if (sqlite3_prepare_v2(db(), sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
  sqlite3_bind_text(stmt, 1, enc_note.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 2, now);
  sqlite3_bind_int(stmt, 3, id);
  bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);
  return ok;
}

vector<Entry> VaultManager::get_entries() {
  vector<Entry> out;
  if (!unlocked) return out;

  // fetch first URL per entry in one pass
  const char *sql =
      "SELECT e.id, e.type, e.title, e.username, e.password_encrypted, e.note_encrypted,"
      "       (SELECT url FROM entry_urls WHERE entry_id=e.id LIMIT 1) as url"
      " FROM entries e ORDER BY e.id DESC;";

  sqlite3_stmt *stmt;
  if (sqlite3_prepare_v2(db(), sql, -1, &stmt, nullptr) != SQLITE_OK) return out;

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Entry e;
    e.id   = sqlite3_column_int(stmt, 0);
    e.type = (const char *)sqlite3_column_text(stmt, 1);
    e.title= (const char *)sqlite3_column_text(stmt, 2);

    const char *user = (const char *)sqlite3_column_text(stmt, 3);
    const char *pass = (const char *)sqlite3_column_text(stmt, 4);
    const char *note = (const char *)sqlite3_column_text(stmt, 5);
    const char *url  = (const char *)sqlite3_column_text(stmt, 6);

    if (user) e.username = user;
    if (pass) e.password = decrypt(string(pass));
    if (note) e.note     = decrypt(string(note));
    if (url)  e.url      = url;

    out.push_back(e);
  }
  sqlite3_finalize(stmt);
  return out;
}

bool VaultManager::delete_entry(int id) {
  sqlite3_stmt *stmt;
  const char *url_sql = "DELETE FROM entry_urls WHERE entry_id=?;";
  sqlite3_prepare_v2(db(), url_sql, -1, &stmt, nullptr);
  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  const char *sql = "DELETE FROM entries WHERE id=?;";
  sqlite3_prepare_v2(db(), sql, -1, &stmt, nullptr);
  sqlite3_bind_int(stmt, 1, id);
  bool ok = (sqlite3_step(stmt) == SQLITE_DONE);
  sqlite3_finalize(stmt);
  return ok;
}