#ifndef TTRS_SPAWNER_H_
#define TTRS_SPAWNER_H_

#include "piece.h"

namespace ttrs {

class spawner {
public:
	virtual ~spawner() { }
	virtual piece spawn_piece() = 0;
	virtual bool has_next_piece() const = 0;
	virtual const piece& next_piece() const = 0;
};

}

#endif
