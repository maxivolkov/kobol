#pragma once
#include "move.h"

struct picker
{
	move mv;
	int16_t score = 0;
};

class movepick
{
public:
	int count = 0;
	move list[228];
	picker scores[228];
	void fill(bool q = false);
	[[nodiscard]] int get_index(const move& m) const;
	picker pick(int index);
	void set_best(int index);
	bool set_move(const move& m);
	void sort();
};
