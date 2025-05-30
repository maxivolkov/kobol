#include <iostream>
#include <vector>
#include "eval.h"
#include "hash.h"
#include "input.h"
#include "movepick.h"
#include "position.h"
#include "search.h"
#include "timeman.h"
#include "uci.h"

void extract_pv(std::vector<move>& list) {
  const uint64_t hash = pos.get_hash();
  const entry* rec = tt.get_entry(hash);
  if (rec == nullptr)
    return;
  const move mv = rec->mv;
  if (!pos.is_legal(mv))
    return;
  pos.make_move(mv);
  if (pos.is_repetition()) {
    pos.unmake_move(mv);
    return;
  }
  list.push_back(mv);
  extract_pv(list);
}

std::string extract_pv() {
  pos.make_move(sd.bmove);
  std::string pv;
  std::vector<move> list;
  extract_pv(list);
  for (int n = static_cast<int>(list.size() - 1); n >= 0; n--) {
    move mv = list[n];
    if (n == 0)
      sd.pmove = mv;
    std::string pvmove = mv.to_uci();
    pv = " " + pvmove + pv;
    pos.unmake_move(mv);
  }
  pos.unmake_move(sd.bmove);
  return sd.bmove.to_uci() + pv;
}

void info_pv() {
  if (sp.ponder || !sp.post)
    return;
  const uint64_t ms = sd.elapsed();
  const uint64_t nps = ms ? sd.nodes * 1000 / ms : 0;
  const std::string score = sd.bscore > near_checkmate
    ? "mate " + std::to_string((max_checkmate - sd.bscore) >> 1)
    : sd.bscore < -near_checkmate
    ? "mate " + std::to_string((-max_checkmate - sd.bscore + 2) >> 1)
    : "cp " + std::to_string(sd.bscore);
  const std::string pv = extract_pv();
  std::cout << "info depth " << sd.depth << " nodes " << sd.nodes << " time " << ms << " nps " << nps << " hashfull " <<
    tt.per_million() << " score " << score << " pv " << pv << std::endl;
}

int32_t qsearch(int32_t alpha, const int32_t beta) {
  if (!(++sd.nodes & 0x1ffff))
    check_time();
  if (sp.game_over)
    return alpha;
  if (const entry* rec = tt.get_entry(pos.get_hash()); rec != nullptr) {
    if (rec->type == node_pv) {
      return rec->score;
    }
    if (rec->type == node_cut) {
      if (rec->score >= beta) {
        return beta;
      }
    }
    else if (rec->type == node_all) {
      if (rec->score <= alpha) {
        return alpha;
      }
    }
  }

  int32_t score = eval();
  if (score >= beta)
    return beta;
  if (score > alpha)
    alpha = score;
  const side color = pos.us();
  movepick picker;
  pos.move_list_q(color, picker.list, picker.count);
  if (!picker.count)
    return alpha;
  picker.fill(true);
  int32_t best_score = -max_checkmate;
  uint16_t best_move = 0;
  const int32_t old_alpha = alpha;

  for (int n = 0; n < picker.count; n++) {
    const move m = picker.pick(n).mv;
    pos.make_move(m);
    score = -qsearch(-beta, -alpha);
    pos.unmake_move(m);
    if (sp.game_over)
      return alpha;
    if (score > best_score) {
      best_score = score;
      best_move = m.mv;
    }
    if (score > alpha)
      alpha = score;
    if (score >= beta)
      break;
  }
  if (!sp.game_over) {
    const entry_type rt = best_score <= old_alpha ? node_all : best_score >= beta ? node_cut : node_pv;
    tt.set_entry(pos.get_hash(), static_cast<int16_t>(best_score), best_move, rt, 0);
  }
  return alpha;
}

int32_t search(int32_t depth, const int32_t ply, int32_t alpha, int32_t beta, const bool do_null) {
  if (pos.move50 >= 100 || pos.is_repetition())
    return ply & 1 ? -options.contempt : options.contempt;
  const side color = pos.us();
  if (pos.is_in_check)
    depth++;
  if (depth < 1)
    return qsearch(alpha, beta);
  if (!(++sd.nodes & 0x1ffff))
    check_time();
  if (sp.game_over)
    return alpha;
  sd.nodesq++;

  const int mate_value = max_checkmate - ply;
  if (alpha < -mate_value) alpha = -mate_value;
  if (beta > mate_value - 1) beta = mate_value - 1;
  if (alpha >= beta) return alpha;

  auto static_eval = static_cast<int16_t>(eval());
  movepick picker;
  pos.move_list(color, picker.list, picker.count);
  picker.fill();

  if (!picker.count)return pos.is_in_check ? -max_checkmate + ply : 0;

  if (const entry* rec = tt.get_entry(pos.get_hash()); rec != nullptr)
    if (picker.set_move(rec->mv)) {
      if (rec->depth >= depth) {
        if (rec->type == node_pv)
          return rec->score;
        if (rec->type == node_cut) {
          if (rec->score >= beta)
            return beta;
        }
        else if (rec->type == node_all)
          if (rec->score <= alpha)
            return alpha;
      }

      if (rec->type == node_pv ||
        (rec->type == node_cut && static_eval < rec->score) ||
        (rec->type == node_all && static_eval > rec->score))
        static_eval = rec->score;
    }

  if (!pos.is_in_check && alpha == beta - 1) {
    if (depth < 5) {
      if (const int margins[] = { 0, 50, 100, 200, 300 }; static_eval - margins[depth] >= beta) {
        return beta;
      }
      if (depth > 2 && static_eval >= beta && do_null && pos.not_only_pawns()) {
        pos.make_null();
        const auto value = static_cast<int16_t>(-search(depth - 4 - depth / 6, ply + 1, -beta, 1 - beta, false));
        pos.unmake_null();
        if (value >= beta)
          return beta;
      }
      if (depth == 1 && static_eval + 200 < alpha)
        return qsearch(alpha, beta);
    }
  }

  int16_t best_score = -max_checkmate;
  uint16_t best_move = 0;
  const auto old_alpha = static_cast<int16_t>(alpha);

  for (int n = 0; n < picker.count; n++) {
    const move m = picker.pick(n).mv;
    pos.make_move(m);
    int32_t score = alpha + 1;
    if (!pos.in_check())
      score = -search(depth - 1, ply + 1, -alpha - 1, -alpha, true);
    if (score > alpha)
      score = -search(depth - 1, ply + 1, -beta, -alpha, true);
    pos.unmake_move(m);
    if (sp.game_over)
      return alpha;
    if (score > best_score) {
      best_score = static_cast<int16_t>(score);
      best_move = m.mv;
    }
    if (score > alpha)
      alpha = score;
    if (score >= beta)
      break;
  }

  if (!sp.game_over) {
    const entry_type rt = best_score <= old_alpha ? node_all : best_score >= beta ? node_cut : node_pv;
    tt.set_entry(pos.get_hash(), best_score, best_move, rt, depth);
  }
  return alpha;
}

int32_t search_root(movepick& picker, const int32_t depth, int32_t alpha, const int32_t beta) {
  int32_t best = -max_checkmate;

  for (int n = 0; n < picker.count; n++) {
    const move m = picker.scores[n].mv;
    pos.make_move(m);
    int32_t score = alpha + 1;

    if (!pos.is_in_check && best > -max_checkmate)
      score = -search(depth - 1, 2, -alpha - 1, -alpha, false);
    if (score > alpha)
      score = -search(depth - 1, 2, -beta, -alpha, false);
    pos.unmake_move(m);

    if (best < score)
      best = score;
    if (sp.game_over)
      return alpha;
    if (score >= beta)
      return beta;
    if (score > alpha) {
      alpha = score;
      picker.set_best(n);
      sd.bmove = m;
      sd.bscore = score;
      info_pv();
    }
  }
  return alpha;
}

int16_t search_widen(movepick& picker, const int16_t depth, int16_t score, uint32_t window) {
  if (pos.is_in_check)
    return static_cast<int16_t>(search_root(picker, depth, -max_checkmate, max_checkmate));
  const auto alpha = static_cast<int32_t>(score - window);
  const auto beta = static_cast<int32_t>(score + window);
  score = static_cast<int16_t>(search_root(picker, depth, alpha, beta));
  if (!sp.game_over && (score <= alpha || score >= beta)) {
    window <<= 1;
    return search_widen(picker, depth, score, window);
  }
  return score;
}

void best_move() {
  if (sp.ponder || !sp.post)
    return;
  info_pv();
  std::cout << "bestmove " << sd.bmove.to_uci();
  if (options.ponder && sd.pmove.mv)
    std::cout << " ponder " << sd.pmove.to_uci();
  std::cout << std::endl;
}

void search_iterate() {
  pos.phase = pos.game_phase();
  sd.restart();
  tt.age++;
  movepick picker;
  pos.move_list(pos.us(), picker.list, picker.count);

  if (!picker.count)
    return;
  if (picker.count == 1) {
    std::cout << "bestmove " << picker.list[0].to_uci() << std::endl;
    return;
  }

  picker.fill();
  picker.sort();

  auto score = static_cast<int16_t>(search(1, 0, -max_checkmate, max_checkmate, false));

  for (sd.depth = 1; sd.depth < max_depth; sd.depth++) {
    score = search_widen(picker, static_cast<int16_t>(sd.depth), score, options.aspiration);
    if (sp.flags & tf_movetime)
      if (sd.elapsed() > static_cast<uint64_t>(sp.movetime / 2))
        break;

    if (sp.flags & tf_depth)
      if (sd.depth >= sp.depth)
        break;

    if (sp.flags & tf_nodes)
      if (sd.nodes >= static_cast<uint64_t>(sp.nodes))
        break;
  }
  best_move();
}
