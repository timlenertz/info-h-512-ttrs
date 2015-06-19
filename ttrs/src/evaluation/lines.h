#ifndef TTRS_LINES_BOARD_EVALUATION_H_
#define TTRS_LINES_BOARD_EVALUATION_H_

#include "../board_evaluation.h"

namespace ttrs {

class lines_board_evaluation : public board_evaluation {
public:
	long state(const board&) const override;
	long number_of_states() const override;
	
	float action_reward(const board&, const board::action&) const override;
};

}

#endif