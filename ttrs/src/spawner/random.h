#ifndef TTRS_RANDOM_SPAWNER_H_
#define TTRS_RANDOM_SPAWNER_H_

#include "../spawner.h"
#include "../piece.h"

namespace ttrs {

class random_spawner : public spawner {
public:
	random_spawner();

	piece spawn_piece();
	bool has_next_piece() const;
	const piece& next_piece() const;

private:
	static piece random_piece_();

	piece next_piece_;
};

}

#endif
