#include "generator.h"
#include <cctype>
#include <random>
using namespace std;

string gen_password(int length, bool upper, bool digits, bool symbols) {
    string pool = "abcdefghijklmnopqrstuvwxyz";
    if (upper)   pool += "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (digits)  pool += "0123456789";
    if (symbols) pool += "!@#$%^&*()-_=+[]{}|;:,.<>?";
    if (pool.empty()) pool = "abcdefghijklmnopqrstuvwxyz";

    random_device rd;
    mt19937 rng(rd());
    uniform_int_distribution<size_t> dist(0, pool.size() - 1);

    string out;
    out.reserve(length);
    for (int i = 0; i < length; i++)
        out += pool[dist(rng)];
    return out;
}

int password_score(const string& pw) {
    if (pw.size() < 8) return 0;
    int score = 0;
    bool has_lower = false, has_upper = false,
         has_digit = false, has_sym   = false;
    for (char c : pw) {
        if (islower((unsigned char)c)) has_lower = true;
        if (isupper((unsigned char)c)) has_upper = true;
        if (isdigit((unsigned char)c)) has_digit = true;
        if (ispunct((unsigned char)c)) has_sym   = true;
    }
    if (pw.size() >= 8)  score++;
    if (pw.size() >= 14) score++;
    if (has_upper && has_digit) score++;
    if (has_sym) score++;
    return min(score, 4);
}