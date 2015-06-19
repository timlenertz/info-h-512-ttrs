#ifndef TTRS_BOARD_REPRESENTATION_H_
#define TTRS_BOARD_REPRESENTATION_H_

#include "board.h"

namespace ttrs {

class board_representation {
public:
	virtual ~board_representation() { }
	
	/// Compute value representing the state that the board is currently in.
	virtual long state(const board&) const = 0;
	
	/// Get possible range of values returned by state function.
	virtual long number_of_states() const = 0;
};

}

#endif
