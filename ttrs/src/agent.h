#ifndef TTRS_AGENT_H_
#define TTRS_AGENT_H_

#include <vector>
#include <string>
#include "board.h"
#include "types.h"

namespace ttrs {

class board_evaluation;
class board_representation;

class agent {
public:
	using action = board::action;
	
	void reset();
	
	void train(game_statistics&);
	void play(game_statistics&);
	
	action greedy_action(bool value_only = false) const;
	
	void set_evaluation(const board_evaluation* ev) { evaluation_ = ev; }
	void set_representation(const board_representation* re) { representation_ = re; }
	void set_board(board* brd) { board_ = brd; }

	float exploration_rate = 0.1;
	float discounting_factor = 0.2;
	float learning_rate = 0.4;
	bool deterministic = true;
	
	void export_values(const std::string& filename) const;
	void import_values(const std::string& filename);
	
private:
	const board_evaluation* evaluation_;
	const board_representation* representation_;
	board* board_;
	
	int games_trained_;
	std::vector<float> state_values_;
};

}

#endif
