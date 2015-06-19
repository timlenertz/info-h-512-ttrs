#include "random.h"
#include <ctime>

namespace ttrs {

namespace {
	std::mt19937 random_number_generator_(std::time(nullptr));
}

std::mt19937& random_number_generator() {
	return random_number_generator_;
}

}