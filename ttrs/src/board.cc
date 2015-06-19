#include "board.h"
#include "piece.h"
#include "spawner.h"
#include <stdexcept>
#include <iostream>
#include <cassert>

namespace ttrs {

namespace {
	template<typename Func>
	void sweep_piece(const piece& pc, Func&& func) {
		const position origin = pc.get_position();
		position pos;
		for(pos[0] = -1; pos[0] <= 2; ++pos[0])
		for(pos[1] = -2; pos[1] <= 1; ++pos[1]) {
			position bpos { origin[0]+pos[0], origin[1]+pos[1] };
			bool cont = func(pos, bpos);
			if(! cont) return;
		}
	}
}

board::board() { }
	
	
board::~board() { }


cell_color board::at(const position& pos, bool with_piece) const {
	std::ptrdiff_t x = pos[1], y = pos[0];
		
	if(x < 0 || x >= width || y >= height) return cell_color::wall;
	else if(y < 0) return cell_color::empty;
	else if(active_piece_ && with_piece) return committed_()[y][x];
	else return cells_[y][x];
}

void board::reset() {
	for(auto& row : cells_) row.fill(cell_color::empty);
	game_over_ = false;
	lines_cleared_ = 0;
	spawn_piece_();
}

void board::rotate_cw() {
	active_piece_.rotate_cw();
}

bool board::can_rotate_cw() const {
	return ! intersection_(active_piece_.rotated_cw());
}

void board::rotate_ccw() {
	active_piece_.rotate_ccw();
}

bool board::can_rotate_ccw() const {
	return ! intersection_(active_piece_.rotated_ccw());
}

void board::shift_left() {
	active_piece_.move(0, -1);
}

bool board::can_shift_left() const {
	return ! intersection_(active_piece_.moved(0, -1));	
}

void board::shift_right() {
	active_piece_.move(0, +1);
}

bool board::can_shift_right() const {
	return ! intersection_(active_piece_.moved(0, +1));	
}

void board::soft_drop() {
	active_piece_.move(+1, 0);
}

bool board::can_soft_drop() const {
	return ! intersection_(active_piece_.moved(+1, 0));	
}

void board::hard_drop() {
	while(can_soft_drop()) soft_drop();
}


void board::tick() {
	if(active_piece_) {
		if(can_fall_()) {
			fall_();
		} else {
			commit_();
			lines_cleared_ += collapse_pile_();
			tick();
		}
	} else if(can_spawn_piece_()) {
		spawn_piece_();
	} else {
		game_over_ = true;
	}
}


int board::lines_cleared() const {
	return lines_cleared_;
}


bool board::intersection_(const piece& pc) const {
	bool inter = false;
	sweep_piece(pc, [&](const position& pos, const position& bpos) {
		inter = (pc.at(pos) && (at(bpos) != cell_color::empty));
		return !inter;
	});
	return inter;
}


void board::commit_() {
	cells_ = committed_();
	active_piece_ = piece();
}


board::cells_matrix board::committed_() const {
	cells_matrix cells = cells_;
	sweep_piece(active_piece_, [&](const position& pos, const position& bpos) {
		if(active_piece_.at(pos)) {
			if(at(bpos) != cell_color::empty)
				throw std::out_of_range("Part of piece out of range.");
		
			std::ptrdiff_t x = bpos[1], y = bpos[0];
			if(y < 0) return true;
			cells[y][x] = cell_color(active_piece_.get_kind());
		}
		return true;
	});
	return cells;
}


bool board::is_full_(const cells_row& row) {
	for(cell_color cell : row) if(cell == cell_color::empty) return false;
	return true;
}


bool board::is_empty_(const cells_row& row) {
	for(cell_color cell : row) if(cell != cell_color::empty) return false;
	return true;
}


int board::collapse_pile_() {
	int collapsed = 0;
	for(std::ptrdiff_t y = height - 1; y >= 0;) {
		if(is_empty_(cells_[y])) {
			break;
		} else if(is_full_(cells_[y])) {
			++collapsed;
			for(std::ptrdiff_t y2 = y - 1; y2 >= 0; --y2) {
				cells_[y2 + 1] = cells_[y2];
				if(is_empty_(cells_[y2])) break;
				else if(y2 == 0) cells_[y2].fill(cell_color::empty);
			}
			
		} else {	
			--y;
		}
	}
	return collapsed;
}


bool board::can_fall_() const {
	return can_soft_drop();
}


void board::fall_() {
	soft_drop();
}


position board::spawned_piece_origin_() const {
	return position { 0, width / 2 };
}

void board::spawn_piece_() {
	active_piece_ = spawner_->spawn_piece();
	active_piece_.set_position(spawned_piece_origin_());
	assert(! intersection_(active_piece_));
}


bool board::can_spawn_piece_() const {
	position origin = spawned_piece_origin_();
	position pos;
	for(pos[0] = origin[0]; pos[0] <= origin[0] + 1; ++pos[0])
	for(pos[1] = origin[1] - 2; pos[1] <= origin[1] + 1; ++pos[1]) {
		if(at(pos) != cell_color::empty) return false;
	}
	return true;
}


std::vector<board::action> board::available_actions() const {
	std::vector<action> actions;
	for(int ori = 0; ori < active_piece_.orientations(); ++ori) {
		for(int pos = 0; pos < width; ++pos) {
			action a { ori, pos };
			if(can_execute_action(a)) actions.push_back(a);
		}
	}
	assert(game_over_ || actions.size() > 0);
	return actions;
}


bool board::can_execute_action(const action& a) const {
	int ori = a.orientation, pos = a.position_x;
	piece copy = active_piece_;
	if(ori < 0 || ori >= copy.orientations()) return false;
	copy.set_orientation(ori);
	copy.set_position(position {0, pos});
	return ! intersection_(copy);
}


void board::execute_action(const action& a) {
	if(! can_execute_action(a)) throw std::invalid_argument("Invalid action.");
	int ori = a.orientation, pos = a.position_x;
	active_piece_.set_orientation(ori);
	active_piece_.set_position(position {0, pos});
	hard_drop();
}


int board::maximal_height() const {
	int maxh = 0;
	for(std::ptrdiff_t x = 0; x != width; ++x) {
		int h = column_height(x);
		if(h > maxh) maxh = h;
	}
	return maxh;
}

int board::column_height(std::ptrdiff_t x) const {
	position pos { 0, x };
	for(; pos[0] != height; ++pos[0]) {
		cell_color c = at(pos);
		if(c != cell_color::empty)
			return height - pos[0];
	}
	return 0;
}

int board::column_holes(std::ptrdiff_t x) const {
	position pos { 0, x };
	int holes = 0;
	for(; pos[0] != height; ++pos[0])
		if(at(pos) != cell_color::empty) break;
	for(; pos[0] != height; ++pos[0])
		if(at(pos) == cell_color::empty) ++holes;
	return holes;
}


}


