#include "Section.h"

Section::Section(std::ifstream& str, size_t max_len) : stream(str)
{
	unsigned int tmpIdent = -1, tmpSize = -1;
	str.read(reinterpret_cast<char*>(&tmpIdent), 4);

	for (auto& section : SectionList) {
		if (section.identifier == tmpIdent || section.identifier == FLIP(tmpIdent))		// test for both endianness
		{
			identifier = tmpIdent;
			str.seekg(section.size_offset, std::ios::cur);								// seek forward with the offset have in the config.
			str.read(reinterpret_cast<char*>(&tmpSize), 4);								// read the actual size value
			if (tmpSize > 0 && tmpSize <= max_len) {
				str.seekg((0-8-section.size_offset), std::ios::cur);					// go back the size of 'size' AND 'identifier' + the offset we have stored in the config for it

				size = tmpSize; data.reserve(tmpSize);
				str.read(reinterpret_cast<char*>(&data.data()[0]), tmpSize);			// read the data from the stream into the std::vector of bytes we have in the class
				data.shrink_to_fit();
			}
			break;
		}
	}
}