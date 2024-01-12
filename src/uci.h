#pragma once
#include <string>
#include <vector>

void uci_command(std::string);
void uci_loop();
void uci_quit();
bool uci_value(const std::vector<std::string>&, const std::string&, std::string&);
std::string trim(const std::string&);
std::vector<std::string>& split_string(const std::string&, std::vector<std::string>&, char);
