#include "agent.h"
#include "board.h"
#include "board_evaluation.h"
#include "random.h"
#include <cmath>
#include <cassert>
#include <vector>
#include <iostream>

namespace ttrs {

void agent::train(int num_games, int num_actions) { return;
	auto& rng = random_number_generator();
	
	board_->reset();
	
	int actions = 0;
	int games_max = games_trained_ + num_games;
	while(games_trained_ != games_max && actions != num_actions) {
		assert(! board_->game_over());
		
		std::uniform_real_distribution<float> dist(0, 1);
		bool explore = (dist(rng) < exploration_rate);
		
		action a;
		float reward; long old_state;
		if(explore) {
			auto actions = board_->available_actions();
			assert(actions.size() > 0);
			
			std::uniform_int_distribution<std::ptrdiff_t> dist(0, actions.size() - 1);
			a = actions.at(dist(rng));
			
		} else {
			a = greedy_action();
			
			reward = evaluation_->action_reward(*board_, a);
			old_state = evaluation_->state(*board_);
		}
		
		board_->execute_action(a);
		board_->tick();
		++actions;

		if(! explore) {
			long new_state = evaluation_->state(*board_);
						
			float old_v = state_values_.at(old_state);
			float new_v = state_values_.at(new_state);
					
			//float add = reward_factor*reward + discounting_factor*new_v - old_v;
			//state_values_.at(old_state) += learning_rate() * add;
		}

		if(board_->game_over()) {
			board_->reset();
			++games_trained_;
		}
	}
}

float agent::learning_rate() const {
	return 1.0f / (1.0f + 15.0f * std::log(games_trained_ + 1));
}

agent::action agent::greedy_action() const {
	auto actions = board_->available_actions();
	if(actions.empty()) throw std::logic_error("No available actions.");
	
	float max_rv = -INFINITY;
	std::vector<action> max_actions;
	
	for(const action& a : actions) {
		board copy = *board_;
		float r = evaluation_->action_reward(*board_, a);
		copy.execute_action(a);
		float v = state_values_.at(evaluation_->state(*board_));
		float rv = reward_factor*r + value_factor*v;

		if(rv > max_rv) {
			max_rv = rv;
			max_actions = {a};
		} else if(rv == max_rv) {
			max_actions.push_back(a);
		}
	}
	
	assert(max_actions.size() > 0);
	if(max_actions.size() == 1) {
		return max_actions[0];
	} else {
		std::uniform_int_distribution<std::ptrdiff_t> dist(0, max_actions.size() - 1);
		std::ptrdiff_t i = dist(random_number_generator());
		return max_actions.at(i);
	}
}

void agent::reset() {
	games_trained_ = 0;
	state_values_ = std::vector<float>(
		evaluation_->number_of_states(),
		0.0
	);
}

}