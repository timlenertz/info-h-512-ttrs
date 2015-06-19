#ifndef TTRS_TOP_COLUMN_HEIGHTS_BOARD_REPRESENTATION_H_
#define TTRS_TOP_COLUMN_HEIGHTS_BOARD_REPRESENTATION_H_

#include "../board_representation.h"

namespace ttrs {

class top_column_heights_board_evaluation : public board_representation {
public:
	int height = 4;

	long state(const board&) const override;
	long number_of_states() const override;
};

}

#endif