#ifndef TTRS_BOARD_EVALUATION_H_
#define TTRS_BOARD_EVALUATION_H_

#include "board.h"

namespace ttrs {

class board_evaluation {
public:
	virtual ~board_evaluation() { }
	
	virtual long state(const board&) const = 0;
	virtual long number_of_states() const = 0;
	
	virtual float action_reward(const board&, const board::action&) const = 0;
};

}

#endif
