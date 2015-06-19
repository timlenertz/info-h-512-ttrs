#ifndef TTRS_BOARD_EVALUATION_H_
#define TTRS_BOARD_EVALUATION_H_

#include "board.h"

namespace ttrs {

class board_evaluation {
public:
	virtual ~board_evaluation() { }
	
	/// Compute value representing the state that the board is currently in.
	virtual long state(const board&) const = 0;
	
	/// Get possible range of values returned by state function.
	virtual long number_of_states() const = 0;
	
	/// Immediate reward of executing given action on board at its current state.
	virtual float action_reward(const board&, const board::action&) const = 0;
};

}

#endif
