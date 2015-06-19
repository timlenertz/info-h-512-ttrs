#ifndef TTRS_BOARD_EVALUATION_H_
#define TTRS_BOARD_EVALUATION_H_

#include "board.h"

namespace ttrs {

class board_evaluation {
public:
	virtual ~board_evaluation() { }
		
	/// Immediate reward of executing given action on board at its current state.
	virtual float action_reward(const board&, const board::action&) const = 0;
};

}

#endif
