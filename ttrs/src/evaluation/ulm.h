#ifndef TTRS_ULM_BOARD_EVALUATION_H_
#define TTRS_ULM_BOARD_EVALUATION_H_

#include "../board_evaluation.h"

namespace ttrs {

class ulm_board_evaluation : public board_evaluation {
public:
	float action_reward(const board&, const board::action&) const override;

private:
	float reward_potential_(const board& brd) const;
};

}

#endif