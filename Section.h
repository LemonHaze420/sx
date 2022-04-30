#pragma once
#include "Helpers.hpp"

struct SectionConfig {
	unsigned int identifier;			// e.g. PVRT
	unsigned long long size_offset;		// e.g. 0 for PVRT (the size comes right after the identifier)
};

class Section {
public:
	Section(std::ifstream& str, size_t max_len);
	~Section() = default;

	std::string					getExtension()	{ return (FOURCC('P', 'V', 'R', 'T') == identifier || FOURCC('P', 'V', 'R', 'T') == FLIP(identifier) ? ".pvr" : std::string("." + FOURST(identifier))); }
	size_t						getSize()		{ return size; }
	std::vector<unsigned char>	getData()		{ return data; }

protected:
	size_t size = 0x0;
	std::vector<unsigned char> data;

	signed int identifier = 0x0;
	std::ifstream& stream;
};
