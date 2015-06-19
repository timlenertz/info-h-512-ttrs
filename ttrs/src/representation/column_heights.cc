#include "column_heights.h"
#include "../piece.h"
#include "../types.h"

namespace ttrs {

long column_heights_board_evaluation::state(const board& brd) const {
	long ind = 0;
	long factor = 1;

	for(std::ptrdiff_t x = 0; x != board::width; ++x) {
		int h = brd.column_height(x);
		int value = (h > max_height ? max_height : h);
		ind += factor * value;
		factor *= (max_height + 1);
	}

	return ind;
}

long column_heights_board_evaluation::number_of_states() const {
	long c = 1;
	for(int i = 0; i != board::width; ++i) c *= (max_height + 1);
	return c;
}

}