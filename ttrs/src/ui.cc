#include "ui.h"
#include "board.h"
#include "evaluation/ulm.h"
#include "evaluation/lines.h"
#include "representation/top_column_heights.h"
#include "representation/column_heights.h"
#include "types.h"
#include "spawner/random.h"
#include "spawner/constant.h"
#include "agent.h"
#include <ncurses.h>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace ttrs {

class ui::impl {
public:
	impl() : board_() { }
	
	void run() {
		board_.set_spawner(&spawner_);
		agent_.set_board(&board_);
		agent_.set_evaluation(&evaluation_);
		agent_.set_representation(&representation_);
		agent_.reset();
		
		main_();
	}
	
private:
	using spawner_type = constant_spawner<piece_kind::dot>;
	using evaluation_type = lines_board_evaluation;
	using representation_type = column_heights_board_evaluation;

	WINDOW* board_window_;
	WINDOW* menu_window_;
	WINDOW* stats_window_;

	board board_;
	evaluation_type evaluation_;
	representation_type representation_;
	spawner_type spawner_;
	agent agent_;

	void main_() {
		initscr();
		
		if(! has_colors()) throw std::runtime_error("No colors.");
		start_color();
		init_pair(1, COLOR_WHITE, COLOR_BLACK); // bg
		init_pair(2, COLOR_BLACK, COLOR_WHITE); // wall
		init_pair(3, COLOR_YELLOW, COLOR_YELLOW); // O
		init_pair(4, COLOR_CYAN, COLOR_CYAN); // I
		init_pair(5, COLOR_GREEN, COLOR_GREEN); // S
		init_pair(6, COLOR_RED, COLOR_RED); // Z
		init_pair(7, COLOR_WHITE, COLOR_WHITE); // L
		init_pair(8, COLOR_BLUE, COLOR_BLUE); // J
		init_pair(9, COLOR_MAGENTA, COLOR_MAGENTA); // T
		init_pair(10, COLOR_BLUE, COLOR_BLUE); // dot
		
		wbkgd(stdscr, COLOR_PAIR(2));
		attron(A_BOLD);
		mvprintw(1, 4, "INFO-H-512 Project  -  Reinforcement Learning Tetris AI");
		attroff(A_BOLD);
		refresh();
		
		menu_window_ = newwin(5, 60, 4, board::width*2 + 10);
		werase(menu_window_);
		wrefresh(menu_window_);
		
		stats_window_ = newwin(board::height - 5, 60, 11, board::width*2 + 10);
		werase(stats_window_);
		wrefresh(stats_window_);

		board_window_ = newwin(board::height + 2, board::width*2 + 2, 4, 4);
		wbkgd(board_window_, COLOR_PAIR(1));
		box(board_window_, 0, 0);		
		draw_board_();
		
		try {
			menu_();
		} catch(const std::exception& ex) {
			endwin();
			std::cout << "Uncaught exception:\n" << ex.what() << std::endl;
			return;
		}
		
		endwin();
	}
	
	void train_() {
		echo();

		int actions = 10000;
		int draw = 0;		
		
		{
			wprintw(menu_window_, "Number of actions: ");
			wscanw(menu_window_, "%i", &actions);
		
			double v;
			werase(menu_window_);
			wprintw(menu_window_, "Exploration rate: ");
			if(wscanw(menu_window_, "%f", &v) != ERR) agent_.exploration_rate = v;
		
			wprintw(menu_window_, "Discounting factor: ");
			if(wscanw(menu_window_, "%f", &v) != ERR) agent_.discounting_factor = v;

			wprintw(menu_window_, "Learning rate: ");
			if(wscanw(menu_window_, "%f", &v) != ERR) agent_.learning_rate = v;
		
			wprintw(menu_window_, "Draw board: ");
			wscanw(menu_window_, "%i", &draw);
		}
		
		werase(menu_window_);
		wrefresh(menu_window_);
		cbreak();
		noecho();
		
		std::vector<game_statistics> stats;
		stats.emplace_back();
		game_statistics* current_stat = &stats.back();
		
		board_.reset();
		for(int i = 0; i < actions; ++i) {
			if(draw || i % 1000 == 0) {
				werase(menu_window_);
				wprintw(
					menu_window_,
						"Training.... %i of %i\n"
						"%i games finished",
					i,
					actions,
					stats.size() - 1
				);
				wrefresh(menu_window_);
			}
		
			agent_.train(*current_stat);
			
			if(draw) {
				draw_board_();
				show_stats_(stats);
			}
			
			if(board_.game_over()) {
				stats.emplace_back();
				current_stat = &stats.back();
				board_.reset();
			}
		}
		
		nocbreak();	
	}
	
	void menu_() {		
		for(;;) {
			nodelay(menu_window_, false);
			noecho();
			cbreak();
			
			werase(menu_window_);
			wprintw(menu_window_,
				"Options: \n"
				"  p) Play            r) Reset agent\n"
				"  t) Train agent     e) Export agent\n"
				"  a) Test agent      i) Import agent\n"
				"  q) Exit"
			);
			wrefresh(menu_window_);

			int ch = wgetch(menu_window_);
			
			werase(menu_window_);
			wrefresh(menu_window_);

			switch(ch) {
			case 'p':
				user_play_();
				break;
			case 't':
				train_();
				break;
			case 'a':
				agent_play_();
				break;
			case 'r':
				agent_.reset();
				break;
			case 'e':
				agent_.export_values("agent.txt");
				break;
			case 'i':
				agent_.import_values("agent.txt");
				break;
			case 'q':
				return;
			}
		}
	}
	
	void agent_play_() {
		double v; int i;
		echo();
		werase(menu_window_);

		wprintw(menu_window_, "Discounting factor: ");
		if(wscanw(menu_window_, "%f", &v) != ERR) agent_.discounting_factor = v;

		noecho();

		werase(menu_window_);
		wprintw(menu_window_,
			"Options: \n"
			"  d) Toggle delay\n"
			"  q) Return to menu"
		);
		wrefresh(menu_window_);

		cbreak();
		noecho();
		
		halfdelay(1);
		bool delay = true;
	
		std::vector<game_statistics> stats;
		stats.emplace_back();
		game_statistics* current_stat = &stats.back();
	
		board_.reset();
		for(;;) {
			agent_.play(*current_stat);
									
			draw_board_();
			
			switch(wgetch(board_window_)) {
			case 'd':
				delay = !delay;
				if(delay) halfdelay(1);
				else { nocbreak(); nodelay(board_window_, TRUE); }
				break;
			case 'q':
				return;
			default:
				break;
			}
						
			if(board_.game_over()) {
				stats.emplace_back();
				current_stat = &stats.back();
				board_.reset();
			}
			
			show_stats_(stats);
		}
		
		nocbreak();
	}
	
	void show_stats_(const std::vector<game_statistics>& stats) {
		werase(stats_window_);
	
		int finished_games = stats.size() - 1;
		if(finished_games > 0) {	
			double avg_acc_reward = 0;
			double avg_num_actions = 0;
			double avg_lines_cleared = 0;
			for(auto&& st = stats.begin(); st != stats.end()-1; ++st) {
				avg_acc_reward += st->accumulated_reward;
				avg_lines_cleared += st->lines_cleared;
				avg_num_actions += st->number_of_actions;
			}
			avg_acc_reward /= finished_games;
			avg_num_actions /= finished_games;
			avg_lines_cleared /= finished_games;
	
			wprintw(
				stats_window_,
					"Finished games: %i\n"
					"Per game:\n"
					"   Avg accumulated reward: %f\n"
					"   Avg lines cleared: %f\n"
					"   Avg actions count: %f\n\n",
				finished_games,
				avg_acc_reward,
				avg_lines_cleared,
				avg_num_actions
			);	
		}

		const game_statistics& st = stats.back();
		wprintw(
			stats_window_,
				"Current game:\n"
				"   Accumulated reward: %f\n"
				"   Lines cleared: %i\n"
				"   Actions count: %i\n"
				"   Reward / action: %f\n",
			st.accumulated_reward,
			st.lines_cleared,
			st.number_of_actions,
			(st.number_of_actions > 0 ? st.accumulated_reward / st.number_of_actions : 0)
		);

		wrefresh(stats_window_);
	}
	
	void user_play_() {
		board_.reset();

		cbreak();
		noecho();
		halfdelay(1);
		keypad(board_window_, true);
		
		bool running = true;
		while(running) {
			draw_board_();
			int ch = wgetch(board_window_);
			if(ch == ERR) {
				board_.tick();
			} else switch(ch) {
				case KEY_LEFT:
					if(board_.can_shift_left()) board_.shift_left();
					break;
				case KEY_RIGHT:
					if(board_.can_shift_right()) board_.shift_right();
					break;
				case KEY_UP:
					if(board_.can_rotate_cw()) board_.rotate_cw();
					break;
				case KEY_DOWN:
					if(board_.can_soft_drop()) board_.soft_drop();
					break;
				case 'x':
					board_.hard_drop();
					break;
				case 'q':
					running = false; break;
			}
		}
		
		nocbreak();
	}
	
	void draw_board_() {
		position pos;
		for(pos[0] = 0; pos[0] != board::height; ++pos[0]) {
			for(pos[1] = 0; pos[1] != board::width; ++pos[1]) {
				cell_color c = board_.at(pos, true);
				int ci = int(c) + 1;
				wattron(board_window_, COLOR_PAIR(ci));
				mvwprintw(board_window_, 1+pos[0], 1+pos[1]*2, "  ");
				wattroff(board_window_, COLOR_PAIR(ci));
			}
		}
		wrefresh(board_window_);
	}
};


ui::ui() : impl_(new impl) { }
ui::~ui() { }

int ui::run(int argc, const char* argv[]) {
	impl_->run();
	return 0;
}



}