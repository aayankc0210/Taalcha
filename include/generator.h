#pragma once
#include <string>
using namespace std;

string gen_password(int length, bool upper, bool digits, bool symbols);
int password_score(const string& pw);