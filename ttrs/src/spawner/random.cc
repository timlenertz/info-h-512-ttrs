#include "random.h"
#include "../random.h"
#include <random>

namespace ttrs {

random_spawner::random_spawner() :
	next_piece_(random_piece_()) { }
	

piece random_spawner::spawn_piece() {
	piece pc = next_piece_;
	next_piece_ = random_piece_();
	return pc;
}

bool random_spawner::has_next_piece() const {
	return true;
}

const piece& random_spawner::next_piece() const {
	return next_piece_;
}


piece random_spawner::random_piece_() {
	auto& rng = random_number_generator();
	std::uniform_int_distribution<int> dist(0, 6);
	piece_kind kind = piece_kind(2 + dist(rng));
	piece pc(kind);
	return pc;
}


}
