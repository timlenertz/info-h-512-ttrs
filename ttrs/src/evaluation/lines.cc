#include "lines.h"
#include "../piece.h"
#include "../types.h"

namespace ttrs {

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