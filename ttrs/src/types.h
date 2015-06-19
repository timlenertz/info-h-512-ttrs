#ifndef TTRS_TYPES_H_
#define TTRS_TYPES_H_

#include <array>
#include <ostream>

namespace ttrs {

enum class piece_kind { none = 0, O = 2, I, S, Z, L, J, T, dot };

enum class cell_color { empty = 0, wall = 1, O = 2, I, S, Z, L, J, T, dot };

using position = std::array<std::ptrdiff_t, 2>;

inline std::ostream& operator<<(std::ostream& str, const position& pos) {
	return str << pos[0] << "," << pos[1];
}

}

#endif
