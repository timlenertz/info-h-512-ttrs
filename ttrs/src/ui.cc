#include "ui.h"
#include "board.h"
#include "evaluation/ulm.h"
#include "evaluation/lines.h"
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
		agent_.reset();
		
		main_();
	}
	
private:
	using spawner_type = random_spawner;//constant_spawner<piece_kind::dot>;
	using evaluation_type = ulm_board_evaluation;//lines_board_evaluation;

	struct game_statistics {
		float accumulated_reward = 0.0;
		int lines_cleared = 0;
		int number_of_actions = 0;
	};

	WINDOW* board_window_;
	WINDOW* menu_window_;
	WINDOW* stats_window_;

	board board_;
	evaluation_type evaluation_;
	spawner_type spawner_;
	agent agent_;
	std::vector<game_statistics> stats_;


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
		werase(menu_window_);
		wprintw(menu_window_, "Number of games: ");
		wrefresh(menu_window_);
		
		int games, actions = -1;
		wscanw(menu_window_, "%i", &games);
		if(games == 0) {
			wprintw(menu_window_, "Number of actions: ");
			wrefresh(menu_window_);
			wscanw(menu_window_, "%i", &actions);

			agent_.train(-1, actions);
			werase(menu_window_);
			wprintw(menu_window_, "Training... %i actions", actions);
			wrefresh(menu_window_);

		} else {
			for(int i = 0; i < games; ++i) {
				agent_.train(1, -1);
				werase(menu_window_);
				wprintw(menu_window_, "Training... game %i of %i", i, games);
				wrefresh(menu_window_);
			}
		}
		
	}
	
	void menu_() {		
		for(;;) {
			noecho();
			nodelay(menu_window_, false);
			
			werase(menu_window_);
			wprintw(menu_window_, "Options: \n  p) Play \n  t) Train agent\n  a) Test agent\n  q) Exit");
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
		
			case 'q':
				return;
			}
		}
	}
	
	void agent_play_() {
		werase(menu_window_);
		wprintw(menu_window_, "Options: \n  d) Toggle delay\n  q) Return to menu");
		wrefresh(menu_window_);
		
		stats_.clear();

		cbreak();
		noecho();
		
		halfdelay(1);
		bool delay = true;
	
		int num_actions = 0;
		float acc_reward = 0;
	
		board_.reset();
		for(;;) {
			auto a = agent_.greedy_action();
			++num_actions;
			acc_reward += evaluation_.action_reward(board_, a);
			board_.execute_action(a);
						
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
			
			board_.tick();
			
			if(board_.game_over()) {
				game_statistics st;
				st.lines_cleared = board_.lines_cleared();
				st.number_of_actions = num_actions;
				st.accumulated_reward = acc_reward;
				board_.reset();
				stats_.push_back(st);
				acc_reward = 0;
				num_actions = 0;
				
				update_stats_();
			}
		}
		
		nocbreak();
	}
	
	void update_stats_() {
		werase(stats_window_);
	
		int n = stats_.size();
		if(n == 0) return;
		
		double avg_acc_reward = 0;
		double avg_num_actions = 0;
		double avg_lines_cleared = 0;
		for(const game_statistics& st : stats_) {
			avg_acc_reward += st.accumulated_reward;
			avg_num_actions += st.number_of_actions;
			avg_lines_cleared += st.lines_cleared;
		}
		avg_acc_reward /= n;
		avg_num_actions /= n;
		avg_lines_cleared /= n;
	
		werase(stats_window_);
		wprintw(
			stats_window_,
			"Games played: %i\nAverage accumulated reward: %f\nAverage lines cleared: %f\nAverage actions count: %f",
			n,
			avg_acc_reward,
			avg_lines_cleared,
			avg_num_actions
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