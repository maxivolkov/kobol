#include <algorithm>
#include <iostream>
#include <ostream>
#include "util.h"

#include "uci.h"

void engine_info() {
  std::cout << engine << " " << version << std::endl;
}

std::string trim(const std::string& s) {
  if (s.empty())
    return s;
  auto start = s.begin();
  while (start != s.end() && std::isspace(*start)) {
    ++start;
  }

  auto end = s.end();
  do {
    --end;
  }
  while (std::distance(start, end) > 0 && std::isspace(*end));

  return std::string(start, end + 1);
}

std::vector<std::string>& split_string(const std::string& txt, std::vector<std::string>& str, const char ch) {
  size_t strpos = txt.find(ch);
  size_t initial_pos = 0;
  str.clear();

  while (strpos != std::string::npos) {
    str.push_back(txt.substr(initial_pos, strpos - initial_pos));
    initial_pos = strpos + 1;

    strpos = txt.find(ch, initial_pos);
  }

  str.push_back(txt.substr(initial_pos, std::min(strpos, txt.size()) - initial_pos + 1));

  return str;
}

std::string str_to_lower(std::string s) {
  std::ranges::transform(s, s.begin(), tolower);
  return s;
}
