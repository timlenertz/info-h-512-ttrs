#ifndef TTRS_UI_H_
#define TTRS_UI_H_

#include <memory>

namespace ttrs {

class ui {
public:
	ui();
	~ui();
	int run(int argc, const char* argv[]);

private:
	class impl;
	std::unique_ptr<impl> impl_;
};

}

#endif
