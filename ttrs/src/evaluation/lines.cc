#include "lines.h"
#include "../piece.h"
#include "../types.h"

namespace ttrs {

long lines_board_evaluation::state(const board& brd) const {
	long ind = 0;
	long factor = 1;

	int maxh = brd.maximal_height();
	for(std::ptrdiff_t x = 0; x != board::width; ++x) {
		int dh = maxh - brd.column_height(x);
		int value = (dh > 4 ? 0 : 4 - dh);
		ind += factor * value;
		factor *= 5;
	}

	return ind;
}

long lines_board_evaluation::number_of_states() const {
	long c = 1;
	for(int i = 0; i != board::width; ++i) c *= 5;
	return c;
}

float lines_board_evaluation::action_reward(const board& brd, const board::action& a) const {
	float old_lc = brd.lines_cleared();
	board copy = brd;
	copy.execute_action(a);
	copy.tick();
	if(copy.game_over()) {
		return -3.0;
	} else {
		return (copy.lines_cleared() - old_lc)*10;
	}
}


}