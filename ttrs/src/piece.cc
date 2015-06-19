#include "piece.h"
#include "board.h"
#include <stdexcept>
#include <cassert>

namespace ttrs {

using shape_bitmap = bool[4][4];

struct piece::shape {
	std::size_t orientations;
	const shape_bitmap* bitmaps;

	shape(std::size_t ori, const shape_bitmap* bmp) :
		orientations(ori), bitmaps(bmp) { }
};


namespace {
	const shape_bitmap O_shape_bitmaps[1] {{
		{0, 0, 0, 0},
		{0, 1, 1, 0},
		{0, 1, 1, 0},
		{0, 0, 0, 0}
	}};
	const piece::shape O_shape(1, O_shape_bitmaps);
	
	const shape_bitmap I_shape_bitmaps[2] {{
		{0, 0, 0, 0},
		{1, 1, 1, 1},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	},{
		{0, 0, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 1, 0}
	}};
	const piece::shape I_shape(2, I_shape_bitmaps);

	const shape_bitmap S_shape_bitmaps[2] {{
		{0, 0, 0, 0},
		{0, 0, 1, 1},
		{0, 1, 1, 0},
		{0, 0, 0, 0}
	},{
		{0, 0, 1, 0},
		{0, 0, 1, 1},
		{0, 0, 0, 1},
		{0, 0, 0, 0}
	}};
	const piece::shape S_shape(2, S_shape_bitmaps);

	const shape_bitmap Z_shape_bitmaps[2] {{
		{0, 0, 0, 0},
		{0, 1, 1, 0},
		{0, 0, 1, 1},
		{0, 0, 0, 0}
	},{
		{0, 0, 0, 1},
		{0, 0, 1, 1},
		{0, 0, 1, 0},
		{0, 0, 0, 0}
	}};
	const piece::shape Z_shape(2, Z_shape_bitmaps);
	
	const shape_bitmap L_shape_bitmaps[4] {{
		{0, 0, 0, 0},
		{0, 1, 1, 1},
		{0, 1, 0, 0},
		{0, 0, 0, 0}
	},{
		{0, 0, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 1, 1},
		{0, 0, 0, 0}
	},{
		{0, 0, 0, 1},
		{0, 1, 1, 1},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	},{
		{0, 1, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 0}
	}};
	const piece::shape L_shape(4, L_shape_bitmaps);
	
	const shape_bitmap J_shape_bitmaps[4] {{
		{0, 0, 0, 0},
		{0, 1, 1, 1},
		{0, 0, 0, 1},
		{0, 0, 0, 0}
	},{
		{0, 0, 1, 1},
		{0, 0, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 0}
	},{
		{0, 1, 0, 0},
		{0, 1, 1, 1},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	},{
		{0, 0, 1, 0},
		{0, 0, 1, 0},
		{0, 1, 1, 0},
		{0, 0, 0, 0}
	}};
	const piece::shape J_shape(4, J_shape_bitmaps);
	
	const shape_bitmap T_shape_bitmaps[4] {{
		{0, 0, 0, 0},
		{0, 1, 1, 1},
		{0, 0, 1, 0},
		{0, 0, 0, 0}
	},{
		{0, 0, 1, 0},
		{0, 0, 1, 1},
		{0, 0, 1, 0},
		{0, 0, 0, 0}
	},{
		{0, 0, 1, 0},
		{0, 1, 1, 1},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	},{
		{0, 0, 1, 0},
		{0, 1, 1, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 0}
	}};
	const piece::shape T_shape(4, T_shape_bitmaps);
	
	const shape_bitmap dot_shape_bitmaps[1] {{
		{0, 0, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 0},
		{0, 0, 0, 0}
	}};
	const piece::shape dot_shape(1, dot_shape_bitmaps);

	
	const piece::shape& shape_for_kind_(piece_kind kind) {
		switch(kind) {
		case piece_kind::O: return O_shape;
		case piece_kind::I: return I_shape;
		case piece_kind::S: return S_shape;
		case piece_kind::Z: return Z_shape;
		case piece_kind::L: return L_shape;
		case piece_kind::J: return J_shape;
		case piece_kind::T: return T_shape;
		case piece_kind::dot: return dot_shape;
		default: throw std::invalid_argument("Invalid kind of piece.");
		}
	}
}


piece::piece() :
	shape_(nullptr),
	kind_(piece_kind::none) { }

piece::piece(piece_kind kind, const position& ps) :
	shape_(&shape_for_kind_(kind)),
	kind_(kind),
	position_(ps),
	orientation_(0) { }


void piece::rotate_cw() {
	if(++orientation_ == orientations()) orientation_ = 0;
}

void piece::rotate_ccw() {
	if(--orientation_ == -1) orientation_ = orientations() - 1;
}


void piece::set_orientation(int ori) {
	if(ori < 0 || ori >= orientations()) throw std::invalid_argument("Invalid orientation.");
	orientation_ = ori;
}

int piece::get_orientation() const {
	return orientation_;
}

void piece::move(std::ptrdiff_t dy, std::ptrdiff_t dx) {
	position_[0] += dy;
	position_[1] += dx;
}

piece piece::rotated_cw() const {
	piece copy = *this;
	copy.rotate_cw();
	return copy;
}

piece piece::rotated_ccw() const {
	piece copy = *this;
	copy.rotate_ccw();
	return copy;
}

piece piece::moved(std::ptrdiff_t dy, std::ptrdiff_t dx) const {
	piece copy = *this;
	copy.move(dy, dx);
	return copy;
}


int piece::orientations() const {
	if(shape_) return shape_->orientations;
	else return 0;
}




bool piece::at(const position& ps) const {
	if(! shape_) return false;
	assert(orientation_ < shape_->orientations);
	std::ptrdiff_t x = ps[1], y = ps[0];
	if(y < -1 || y > 2 || x < -2 || x > 1) return false;
	else return shape_->bitmaps[orientation_][y + 1][x + 2];
}



}