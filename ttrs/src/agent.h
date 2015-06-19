#ifndef TTRS_AGENT_H_
#define TTRS_AGENT_H_

#include <vector>
#include "board.h"

namespace ttrs {

class board_evaluation;

class agent {
public:
	using action = board::action;
	
	void reset();
	void train(int games, int actions = -1);
	action greedy_action() const;
	
	void set_evaluation(const board_evaluation* ev) { evaluation_ = ev; }
	void set_board(board* brd) { board_ = brd; }

	float exploration_rate = 0.1;
	
	float reward_factor = 1.0;
	float value_factor = 0.0;

	float learning_rate() const;
	
private:
	const board_evaluation* evaluation_;
	board* board_;
	
	int games_trained_;
	std::vector<float> state_values_;
};

}

#endif
