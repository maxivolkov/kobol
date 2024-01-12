#include <iostream>
#include <string>
#include <vector>
#include "uci.h"

void uci_command(std::string str)
  {
  str = trim(str);
  std::string value;
  std::vector<std::string> split{};
  split_string(str, split, ' ');

  if (split.empty())
    return;

  if (const std::string command = split[0]; command == "uci")
    {
    std::cout << "id name kobol\n";
    std::cout << "id author maksim\n";
    std::cout << "uciok" << std::endl;
    }
  else if (command == "isready")
    {
    std::cout << "readyok" << std::endl;
    }
  else if (command == "ucinewgame")
    {
    }
  else if (command == "position")
    {
    }
  else if (command == "go")
    {
    if (std::string token; uci_value(split, "go", token))
      {
      if (token == "infinite")
        {
        }
      if (token == "ponder")
        {
        }
      }
    if (uci_value(split, "wtime", value))
      {
      }
    if (uci_value(split, "btime", value))
      {
      }
    if (uci_value(split, "winc", value))
      {
      }
    if (uci_value(split, "binc", value))
      {
      }
    if (uci_value(split, "movestogo", value))
      {
      }
    if (uci_value(split, "depth", value))
      {
      }
    if (uci_value(split, "nodes", value))
      {
      }
    if (uci_value(split, "movetime", value))
      {
      }
    }
  else if (command == "ponderhit")
    {
    }
  else if (command == "quit")
    {
    uci_quit();
    }
  else if (command == "stop")
    {
    }
  else if (command == "setoption")
    {
    }
  else if (command == "bench")
    {
    }
  else if (command == "perft")
    {
    }
  }

void uci_loop()
  {
  std::string str;
  while (true)
    {
    getline(std::cin, str);
    uci_command(str);
    }
  }

void uci_quit()
  {
  exit(0);
  }

bool uci_value(const std::vector<std::string>& list, const std::string& command, std::string& value)
  {
  value = "";
  for (size_t n = 0; n < list.size() - 1; n++)
    if (list[n] == command)
      {
      value = list[n + 1];
      return true;
      }
  return false;
  }

std::string trim(const std::string& s)
  {
  if (s.empty())
    return s;
  auto start = s.begin();
  while (start != s.end() && std::isspace(*start))
    {
    ++start;
    }

  auto end = s.end();
  do
    {
    --end;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
  }

std::vector<std::string>& split_string(const std::string& txt, std::vector<std::string>& str, const char ch)
  {
  size_t strpos = txt.find(ch);
  size_t initial_pos = 0;
  str.clear();

  while (strpos != std::string::npos)
    {
    str.push_back(txt.substr(initial_pos, strpos - initial_pos));
    initial_pos = strpos + 1;

    strpos = txt.find(ch, initial_pos);
    }

  str.push_back(txt.substr(initial_pos, std::min(strpos, txt.size()) - initial_pos + 1));

  return str;
  }