#include "ulm.h"
#include "../piece.h"
#include "../types.h"

namespace ttrs {

float ulm_board_evaluation::action_reward(const board& brd, const board::action& a) const {
	float old_r = reward_potential_(brd);
	board copy = brd;
	copy.execute_action(a);
	copy.tick();
	float new_r = reward_potential_(copy);
	
	if(new_r < old_r) return 10*(old_r - new_r);
	else return old_r - new_r;
}

float ulm_board_evaluation::reward_potential_(const board& brd) const {
	float avg_column_height = 0;
	float nb_holes = 0;
	float quadratic_unevenness = 0;
		
	int prev_col_height;
	for(std::ptrdiff_t x = 0; x != board::width; ++x) {
		int col_height = brd.column_height(x);
		avg_column_height += col_height;
		
		nb_holes += brd.column_holes(x);
		
		if(x > 0) {
			float diff = prev_col_height - col_height;
			quadratic_unevenness += diff * diff;
		}
		
		prev_col_height = col_height;
	}
	
	avg_column_height /= board::width;
	quadratic_unevenness /= (board::width - 1);
	
	return 5*avg_column_height + 16*nb_holes + 1*quadratic_unevenness;
}


}