#ifndef TTRS_PIECE_H_
#define TTRS_PIECE_H_

#include "types.h"

namespace ttrs {

class piece {
public:
	piece();
	explicit piece(piece_kind, const position& ps = position());
	piece(const piece&) = default;

	explicit operator bool () const { return (kind_ != piece_kind::none); }

	void rotate_cw();
	void rotate_ccw();
	void move(std::ptrdiff_t dy, std::ptrdiff_t dx);

	piece rotated_cw() const;
	piece rotated_ccw() const;
	piece moved(std::ptrdiff_t dy, std::ptrdiff_t dx) const;

	int orientations() const;
	void set_orientation(int);
	int get_orientation() const;

	bool at(const position&) const;

	void set_position(const position& pos) { position_ = pos; }
	const position& get_position() const { return position_; }
	
	piece_kind get_kind() const { return kind_; }

	struct shape;

private:
	const shape* shape_;
	piece_kind kind_;
	position position_;
	int orientation_;
};

}

#endif