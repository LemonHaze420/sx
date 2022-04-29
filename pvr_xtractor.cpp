/*
		(S)ection (X)tractor	- LemonHaze - 2022
*/
#include "Helpers.hpp"

// Files extensions to explicitly exclude when processing files for sections
std::vector <std::string> exclude_extensions = {
	".EMU",
};

// List of Sections to extract
std::vector<unsigned int> SectionList = {
	FOURCC('P','V','R','T')
};

class Section {
public:
	Section(std::ifstream& str, size_t max_len) : stream(str) 
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
	~Section() = default;

	std::string getExtension() {
		return (SectionList[0] == identifier || SectionList[0] == FLIP(identifier) ? ".pvr" : FOURST(identifier));
	}

	size_t size = 0x0;
	std::vector<unsigned char> data;

protected:
	signed int identifier = 0x0;
	std::ifstream& stream;
};

int main(int argc, char ** argp)
{
	bool bVerbose = true;

	std::string output_dir;
	if (argc == 3)
		output_dir = argp[2];

	auto files = FindFilesOfExtension(argp[1]);
	int num_file = 0, num_completed = 0, num_failed = 0;
	for (auto& file : files) {
		printf("Processing %s..\n", file.c_str());
		
		std::string output_path = file;
		replace(output_path, argp[1], output_dir);

		std::ifstream fileStream(file, std::ios::binary);
		if (fileStream.good()) {
			int totalSize = -1;
			fileStream.seekg(0, std::ios::end);
			totalSize = (int)fileStream.tellg();
			fileStream.seekg(0, std::ios::beg);

			while (!fileStream.eof() && fileStream.good()) {
				Section tmpSection(fileStream, totalSize);
				if (tmpSection.data.size() > 0) {
					num_file++;

					std::string filename = GetFilename(file, false).append("_").append(std::to_string(num_file)).append(tmpSection.getExtension());
					
					std::string path = std::filesystem::path(output_path).parent_path().string().append("\\" + filename).c_str();
					std::filesystem::create_directories(std::filesystem::path(output_path).parent_path());
					
					std::ofstream output_file(path, std::ios::binary);
					if (output_file.good()) {
						printf("Writing to %s..\n", path.c_str());
						output_file.write(reinterpret_cast<char*>(&tmpSection.data.data()[0]), tmpSection.size);
						output_file.close();
						++num_completed;
					} else {
						printf("Unable to write to %s..\n", path.c_str());
						++num_failed;
					}
				}
			}
			if (fileStream.eof() || !fileStream.good())
				continue;
		}
		fileStream.close();
	}


	printf("All tasks completed with %d errors. (%d out of %d).\n", num_failed, num_completed, num_file);
}