#include "VaultManager.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <cstring>
using namespace std;

static string to_hex(const uint8_t* data, size_t len) {
    ostringstream ss;
    for (size_t i = 0; i < len; i++)
        ss << hex << setw(2) << setfill('0') << (int)data[i];
    return ss.str();
}

static vector<uint8_t> from_hex(const string& hex) {
    vector<uint8_t> bytes;
    for (size_t i = 0; i + 1 < hex.size(); i += 2)
        bytes.push_back((uint8_t)stoi(hex.substr(i, 2), nullptr, 16));
    return bytes;
}

bool VaultManager::derive_key(const string& password, const string& salt_hex) {
    string input = password + salt_hex;
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256((const uint8_t*)input.data(), input.size(), hash);
    master_key.assign(hash, hash + 16);
    return true;
}

string VaultManager::hash_password(const string& password) {
    uint8_t hash[SHA256_DIGEST_LENGTH];
    SHA256((const uint8_t*)password.data(), password.size(), hash);
    return to_hex(hash, SHA256_DIGEST_LENGTH);
}

bool VaultManager::verify_password(const string& password, const string& stored_hash) {
    return hash_password(password) == stored_hash;
}

string VaultManager::encrypt(const string& msg) {
    if (master_key.size() != 16) return "";

    uint8_t iv[16];
    RAND_bytes(iv, 16);

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, master_key.data(), iv);

    vector<uint8_t> cipher(msg.size() + 16);
    int out_len = 0, final_len = 0;
    EVP_EncryptUpdate(ctx, cipher.data(), &out_len, (const uint8_t*)msg.data(), (int)msg.size());
    EVP_EncryptFinal_ex(ctx, cipher.data() + out_len, &final_len);
    EVP_CIPHER_CTX_free(ctx);

    cipher.resize(out_len + final_len);
    return to_hex(iv, 16) + to_hex(cipher.data(), cipher.size());
}

string VaultManager::decrypt(const string& hex) {
    if (master_key.size() != 16 || hex.size() < 64) return "";

    auto bytes = from_hex(hex);
    if (bytes.size() < 32) return "";

    uint8_t iv[16];
    memcpy(iv, bytes.data(), 16);

    size_t cipher_len = bytes.size() - 16;

    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), nullptr, master_key.data(), iv);

    vector<uint8_t> plain(cipher_len + 16);
    int out_len = 0, final_len = 0;
    EVP_DecryptUpdate(ctx, plain.data(), &out_len, bytes.data() + 16, (int)cipher_len);
    int ok = EVP_DecryptFinal_ex(ctx, plain.data() + out_len, &final_len);
    EVP_CIPHER_CTX_free(ctx);

    if (ok != 1) return "";
    plain.resize(out_len + final_len);
    return string((char*)plain.data(), plain.size());
}