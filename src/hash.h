#pragma once
#include <cstring>
#include "uci.h"

enum entry_type :uint8_t {
  node_pv = 0,
  node_cut = 1,
  node_all = 2
};

struct entry {
  uint64_t hash; //8
  uint16_t mv; //2
  int16_t score; //2
  uint8_t depth; //1
  entry_type type; //1 
  uint16_t age; //2

  void entry_data(const uint64_t h, const int16_t s, const uint16_t m, const entry_type t, const uint8_t d,
    const uint16_t a) {
    hash = h;
    score = s;
    mv = m;
    type = t;
    depth = d;
    age = a;
  }
};

class hash_table {
  uint64_t used_{};
  uint64_t size_{};
  uint64_t mask_{};
  entry* tt_{};

public:
  uint16_t age{};

  hash_table() {
    resize(options.hash);
  }

  ~hash_table() {
    delete tt_;
  }

  void clear() {
    used_ = 0;
    std::memset(tt_, 0, sizeof(entry) * size_);
  }

  [[nodiscard]] int per_million() const {
    return static_cast<int>(used_ * 1000ul / size_);
  }

  void resize(const uint64_t mb_size) {
    size_ = 1;
    while (size_ <= mb_size)
      size_ <<= 1;
    size_ = (size_ << 20) / sizeof(entry) / 2;
    mask_ = size_ - 1;
    free(tt_);
    tt_ = static_cast<entry*>(calloc(size_, sizeof(entry)));
    clear();
  }

  bool set_entry(const uint64_t hash, const int16_t score, const uint16_t mv, const entry_type type, const int depth) {
    const uint64_t index = hash & mask_;
    entry* en_p = &tt_[index];
    if (!en_p->hash) {
      en_p->entry_data(hash, score, mv, type, static_cast<uint8_t>(depth), age);
      used_++;
      return true;
    }
    if (
      en_p->age != age
      || type == node_pv
      || (en_p->type != node_pv && en_p->depth <= depth)) {
      en_p->entry_data(hash, score, mv, type, static_cast<uint8_t>(depth), age);
      return true;
    }
    return false;
  }

  [[nodiscard]] entry* get_entry(const uint64_t hash) const {
    const uint64_t index = hash & mask_;
    if (entry* en_p = &tt_[index]; en_p->hash == hash)
      return en_p;
    return nullptr;
  }
};

inline hash_table tt;
