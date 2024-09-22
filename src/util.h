#pragma once
#include <string>
#include <vector>

void engine_info();
std::string trim(const std::string& s);
std::vector<std::string>& split_string(const std::string& txt, std::vector<std::string>& str, char ch);
std::string str_to_lower(std::string s);
