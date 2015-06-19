#ifndef TTRS_BOARD_H_
#define TTRS_BOARD_H_

#include "types.h"
#include "piece.h"
#include <memory>
#include <utility>
#include <array>
#include <vector>

namespace ttrs {

class spawner;

class board {
public:
	constexpr static std::ptrdiff_t width = 10;
	constexpr static std::ptrdiff_t height = 20;

	struct action {
		int orientation;
		int position_x;
	};

	board();
	~board();

	void reset();
	
	void tick();

	void rotate_cw();
	bool can_rotate_cw() const;
	void rotate_ccw();
	bool can_rotate_ccw() const;
	void shift_left();
	bool can_shift_left() const;
	void shift_right();
	bool can_shift_right() const;
	void soft_drop();
	bool can_soft_drop() const;
	
	void hard_drop();
	
	int lines_cleared() const;

	void set_spawner(spawner* spw) { spawner_ = spw; }
	
	cell_color at(const position& pos, bool with_piece = false) const;
	
	bool game_over() const { return game_over_; }
	
	/// Get set of possible actions that can be executed at current state.
	/// Action = immediately put new incoming piece at certain X position and certain orientation,
	/// and then do hard drop. Does not correspond 1:1 to actions a human player can do.
	std::vector<action> available_actions() const;
	
	bool can_execute_action(const action&) const;
	void execute_action(const action&);
	
	int maximal_height() const;
	int column_height(std::ptrdiff_t x) const;
	int column_holes(std::ptrdiff_t x) const;
		
protected:
	using cells_row = std::array<cell_color, width>;
	using cells_matrix = std::array<cells_row, height>;

	bool intersection_(const piece&) const;

	bool can_fall_() const;
	void fall_();
	
	void commit_();
	cells_matrix committed_() const;
	int collapse_pile_();
	
	bool can_spawn_piece_() const;
	void spawn_piece_();
	position spawned_piece_origin_() const;

	static bool is_full_(const cells_row&);
	static bool is_empty_(const cells_row&);

	cells_matrix cells_;
	piece active_piece_;
	int lines_cleared_;
	bool game_over_;
	
	spawner* spawner_;
};

}

#endif
