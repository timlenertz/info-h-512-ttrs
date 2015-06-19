#include "top_column_heights.h"
#include "../piece.h"
#include "../types.h"

namespace ttrs {

long top_column_heights_board_evaluation::state(const board& brd) const {
	long ind = 0;
	long factor = 1;

	int maxh = brd.maximal_height();
	for(std::ptrdiff_t x = 0; x != board::width; ++x) {
		int dh = maxh - brd.column_height(x);
		int value = (dh > height ? 0 : height - dh);
		ind += factor * value;
		factor *= (height + 1);
	}

	return ind;
}

long top_column_heights_board_evaluation::number_of_states() const {
	long c = 1;
	for(int i = 0; i != board::width; ++i) c *= (height + 1);
	return c;
}

}