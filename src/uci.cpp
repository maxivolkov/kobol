#include <iostream>
#include <vector>
#include "eval.h"
#include "hash.h"
#include "position.h"
#include "timeman.h"
#include "util.h"
#include "uci.h"

bool uci_value(const std::vector<std::string>& list, const std::string& command, std::string& value) {
  value = "";
  for (size_t n = 0; n < list.size() - 1; n++)
    if (list[n] == command) {
      value = list[n + 1];
      return true;
    }
  return false;
}

uint64_t perft(const int depth) {
  uint64_t nodes = 0;
  int count;
  move list[256];
  pos.move_list(pos.color_us(), list, count);
  if (depth == 1)
    return count;
  for (int i = 0; i < count; i++) {
    pos.make_move(list[i]);
    nodes += perft(depth - 1);
    pos.unmake_move(list[i]);
  }
  return nodes;
}

void uci_perft(const int depth) {
  pos.set_fen(start_fen);
  std::cout << pos << '\n';

  for (int d = 1; d <= depth; d++) {
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    const uint64_t nodes = perft(d);
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    uint64_t time = duration.count();
    if (time < 1) time = 1;
    const uint64_t nps = nodes * 1000 / time;
    std::cout << "depth " << d << " nodes " << nodes << " time " << time << " nps " << nps << std::endl;
  }
}

void uci_bench(const int depth) {
  pos.set_fen(start_fen);
  std::cout << pos << '\n';
  sp.post = false;
  sp.flags = tf_depth;
  for (int d = 1; d <= depth; d++) {
    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
    sp.depth = d;
    search_iterate();
    const uint64_t nodes = sd.nodes;
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin);
    uint64_t time = duration.count();
    if (time < 1) time = 1;
    const uint64_t nps = nodes * 1000 / time;
    std::cout << "depth " << d << " time " << time << " nodes " << nodes << " nps " << nps << std::endl;
  }
}

void uci_eval() {
  const auto score = static_cast<uint16_t>(eval(pos));
  std::cout << "score " << score << std::endl;
}

void uci_ponderhit() {
  sp.ponder = false;
  sp.flags &= ~tf_infinite;
  sd.time_start = std::chrono::steady_clock::now();
}

void uci_quit() {
  exit(0);
}

void uci_stop() {
  sp.game_over = true;
}

void uci_command(std::string str) {
  str = trim(str);
  std::string value;
  std::vector<std::string> split{};
  split_string(str, split, ' ');

  if (split.empty())
    return;

  if (const std::string command = split[0]; command == "uci") {
    std::cout << "id name kobol\n";
    std::cout << "id author maksim\n";
    std::cout << "option name Hash type spin default " << options.hash << " min 64 max 65536\n";
    std::cout << "option name Contempt type spin default " << options.contempt << " min -50 max 50\n";
    std::cout << "option name Ponder type check default " << options.ponder << " true false\n";
    std::cout << "uciok" << std::endl;
  }
  else if (command == "isready")
    std::cout << "readyok" << std::endl;

  else if (command == "ucinewgame")
    tt.clear();

  else if (command == "position") {
    bool bfen = false;
    bool bmov = false;
    int i = 0;
    std::string fen;
    std::vector<std::string> moves = {};
    while (static_cast<uint64_t>(i) < split.size()) {
      if (bfen)
        fen += ' ' + split[i];
      if (bmov)
        moves.push_back(split[i]);
      if (split[i] == "fen")
        bfen = true;
      else if (split[i] == "moves")
        bmov = true;
      i++;
    }
    fen = trim(fen);
    pos.set_fen(fen.empty() ? start_fen : fen);
    for (const std::string& uci : moves) {
      for (auto list = move_list(pos); move mv : list)
        if (mv.to_uci() == uci)
          pos.make_move(mv);
    }
  }

  else if (command == "go") {
    sp.reset();
    if (std::string token; uci_value(split, "go", token)) {
      if (token == "infinite")
        sp.flags |= tf_infinite;
      if (token == "ponder") {
        sp.ponder = true;
        sp.flags |= tf_infinite;
      }
    }
    if (uci_value(split, "wtime", value)) {
      sp.flags |= tf_time;
      sp.time[white] = stoi(value);
    }
    if (uci_value(split, "btime", value)) {
      sp.flags |= tf_time;
      sp.time[black] = stoi(value);
    }
    if (uci_value(split, "winc", value)) {
      sp.flags |= tf_inc;
      sp.inc[white] = stoi(value);
    }
    if (uci_value(split, "binc", value)) {
      sp.flags |= tf_inc;
      sp.inc[black] = stoi(value);
    }
    if (uci_value(split, "movestogo", value)) {
      sp.flags |= tf_movestogo;
      sp.movestogo = stoi(value);
    }
    if (uci_value(split, "depth", value)) {
      sp.flags |= tf_depth;
      sp.depth = stoi(value);
    }
    if (uci_value(split, "nodes", value)) {
      sp.flags |= tf_nodes;
      sp.nodes = stoi(value);
    }
    if (uci_value(split, "movetime", value)) {
      sp.flags |= tf_movetime;
      sp.movetime = stoi(value);
    }
    if (!sp.flags)
      sp.flags |= tf_infinite;
    if (sp.flags & tf_time) {
      sp.flags |= tf_movetime;
      if (sp.movestogo)
        sp.movetime = sp.time[pos.color_us()] / sp.movestogo;
      else
        sp.movetime = sp.time[pos.color_us()] / 32 + sp.inc[pos.color_us()] / 2;
      sp.movetime = sp.movetime - options.overhead;
    }
    search_iterate();
  }

  else if (command == "ponderhit")
    uci_ponderhit();

  else if (command == "quit")
    uci_quit();

  else if (command == "stop")
    uci_stop();

  else if (command == "setoption") {
    const bool val = uci_value(split, "value", value);

    if (std::string name; val && uci_value(split, "name", name)) {
      if (name == "Contempt")
        options.contempt = stoi(value) * 10;

      else if (name == "Hash")
        tt.resize(stoi(value));

      else if (name == "Ponder")
        options.ponder = value == "true";
    }
  }

  else if (command == "bench") {
    if (uci_value(split, "bench", value))
      uci_bench(stoi(value));
    else
      uci_bench(12);
  }

  else if (command == "perft") {
    if (uci_value(split, "perft", value))
      uci_perft(stoi(value));
    else
      uci_perft(7);
  }

  else if (command == "eval")
    uci_eval();

  else if (command == "d")
    std::cout << pos;
}

void uci_loop() {
  pos.set_fen();
  std::string str;
  while (true) {
    getline(std::cin, str);
    uci_command(str);
  }
}
