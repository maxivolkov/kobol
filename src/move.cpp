#include "main.h"
#include "move.h"

template <>
inline move* make<promos>(const square from, uint64_t to, move* list)
{
	while (to)
	{
		const square p = pop_lsb(&to);
		*list++ = move(from, p, promo_knight);
		*list++ = move(from, p, promo_bishop);
		*list++ = move(from, p, promo_rook);
		*list++ = move(from, p, promo_queen);
	}
	return list;
}

template <>
inline move* make<promo_caps>(const square from, uint64_t to, move* list)
{
	while (to)
	{
		const square p = pop_lsb(&to);
		*list++ = move(from, p, promo_cap_knight);
		*list++ = move(from, p, promo_cap_bishop);
		*list++ = move(from, p, promo_cap_rook);
		*list++ = move(from, p, promo_cap_queen);
	}
	return list;
}