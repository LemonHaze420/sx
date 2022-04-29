#pragma once
#include "Helpers.hpp"

class Section {
public:
	Section(std::ifstream& str, size_t max_len);
	~Section() = default;

	std::string getExtension() {
		return (FOURCC('P', 'V', 'R', 'T') == identifier || FOURCC('P', 'V', 'R', 'T') == FLIP(identifier) ? ".pvr" : FOURST(identifier));
	}

	size_t size = 0x0;
	std::vector<unsigned char> data;

protected:
	signed int identifier = 0x0;
	std::ifstream& stream;
};
