#pragma once
#include <string>

std::string gen_password(int length, bool upper, bool digits, bool symbols);
int password_score(const std::string& pw);
