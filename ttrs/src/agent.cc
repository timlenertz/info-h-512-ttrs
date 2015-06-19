#include "agent.h"
#include "board.h"
#include "board_evaluation.h"
#include "board_representation.h"
#include "random.h"
#include <cmath>
#include <cassert>
#include <vector>
#include <iostream>
#include <fstream>
#include <type_traits>

namespace ttrs {

void agent::train(game_statistics& stat) {
	auto& rng = random_number_generator();
	
	std::uniform_real_distribution<float> dist(0, 1);
	bool explore = (dist(rng) < exploration_rate);

	action a;	
	if(explore) {
		// Exploration: Take random action
		auto actions = board_->available_actions();
		assert(actions.size() > 0);
			
		std::uniform_int_distribution<std::ptrdiff_t> dist(0, actions.size() - 1);
		a = actions.at(dist(rng));

	} else {
		// Exploitation: Take greedy action
		a = greedy_action();
	}
	
	float reward = evaluation_->action_reward(*board_, a);
	long old_state = representation_->state(*board_);
	
	board_->execute_action(a);
	board_->tick();
	
	stat.accumulated_reward += reward;
	stat.lines_cleared = board_->lines_cleared();
	stat.number_of_actions++;
	
	if(! explore) {
		// Exploitation
		// Greedy action was taken. Now update state_values_ to improve its prediction.
		long new_state = representation_->state(*board_);
		
		float old_v = state_values_.at(old_state);
		float new_v = state_values_.at(new_state);
					
		float temporal_difference_error = reward + discounting_factor*new_v - old_v;
		state_values_.at(old_state) += learning_rate * temporal_difference_error;
	}
}

void agent::play(game_statistics& stat) {
	action a = greedy_action();
	float reward = evaluation_->action_reward(*board_, a);
	board_->execute_action(a);
	board_->tick();

	stat.accumulated_reward += reward;
	stat.lines_cleared = board_->lines_cleared();
	stat.number_of_actions++;
}


agent::action agent::greedy_action(bool value_only) const {
	auto actions = board_->available_actions();
	if(actions.empty()) throw std::logic_error("No available actions.");
	
	float max_rv = -INFINITY;
	std::vector<action> max_actions;
	
	for(const action& a : actions) {
		float rv;
		float r = evaluation_->action_reward(*board_, a);
		board copy = *board_;
		copy.execute_action(a);
		copy.tick();
		float v = state_values_.at(representation_->state(copy));

		if(value_only) rv = v;
		else rv = r + discounting_factor*v;

		if(rv > max_rv) {
			max_rv = rv;
			max_actions = {a};
		} else if(rv == max_rv) {
			max_actions.push_back(a);
		}
	}
	
	assert(max_actions.size() > 0);
	if(max_actions.size() == 1 || deterministic) {
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
		representation_->number_of_states(),
		0.0
	);
}

void agent::export_values(const std::string& filename) const {
	std::ofstream str(filename);
	for(float v : state_values_) str << v << '\n';
}

void agent::import_values(const std::string& filename) {
	std::ifstream str(filename);
	state_values_.clear();
	float v;
	while(str >> v) {
		state_values_.push_back(v);
		str.ignore(std::numeric_limits<std::streamsize>::max(), '\n');	
	}
}

}