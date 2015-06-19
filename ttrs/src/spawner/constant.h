#ifndef TTRS_CONSTANT_SPAWNER_H_
#define TTRS_CONSTANT_SPAWNER_H_

#include "../spawner.h"
#include "../piece.h"

namespace ttrs {

template<piece_kind Kind>
class constant_spawner : public spawner {
public:
	piece spawn_piece() { return piece_; }
	bool has_next_piece() const { return true; }
	const piece& next_piece() const { return piece_; }

private:
	const piece piece_ = piece(Kind);
};

}

#endif
