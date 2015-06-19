#ifndef TTRS_COLUMN_HEIGHTS_BOARD_REPRESENTATION_H_
#define TTRS_COLUMN_HEIGHTS_BOARD_REPRESENTATION_H_

#include "../board_representation.h"

namespace ttrs {

class column_heights_board_evaluation : public board_representation {
public:
	int max_height = 1;

	long state(const board&) const override;
	long number_of_states() const override;
};

}

#endif