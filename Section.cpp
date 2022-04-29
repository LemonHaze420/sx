#include "Section.h"

Section::Section(std::ifstream& str, size_t max_len) : stream(str)
{
	unsigned int tmpIdent = -1, tmpSize = -1;
	str.read(reinterpret_cast<char*>(&tmpIdent), 4);

	for (auto& section : SectionList) {
		if (section == tmpIdent || section == FLIP(tmpIdent))
		{
			identifier = tmpIdent;
			str.read(reinterpret_cast<char*>(&tmpSize), 4);
			if (tmpSize > 0 && tmpSize <= max_len) {
				str.seekg(-8, std::ios::cur); // we've read 2 * 4 bytes so far, so we need to go back 8..

				size = tmpSize; data.reserve(tmpSize);
				str.read(reinterpret_cast<char*>(&data.data()[0]), tmpSize);
				data.shrink_to_fit();
			}
			break;
		}
	}
}