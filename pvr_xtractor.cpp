/*
		(S)ection (X)tractor


		Aims to extract "sections" from a directory of files. A "section" is defined as anything which resembles the following structure:-

		*---------------------------------------------------------------------------------------*
		| Offset		|	Size	|	Value													|
		*---------------------------------------------------------------------------------------*
		| 0x0			|	 4		|	FOURCC Identifier (ie. 'PVRT' for PowerVR textures)		|
		| 0x4			|	 4		|	Size of section in bytes								|
		*---------------------------------------------------------------------------------------*
		
		Files are written to the input/current directory, or optionally to another directory with the third argument.

		Example usage:-

			* sx.exe "/path/to/files"
			
			* sx.exe "/path/to/files" "/path/to/output"
*/


#define FOURST(x)		 std::string(reinterpret_cast<char*>(&x), 4)
#define FOURCC(a,b,c,d)	 (signed int)((char)(a & 0xFF) << 24 | (char)(b & 0xFF) << 16 | (char)(c & 0xFF) << 8 | (char)(d & 0xFF))
#define FLIP(x)			 (signed int)(( x >> 24 ) | (( x << 8) & 0x00ff0000 )| ((x >> 8) & 0x0000ff00) | ( x << 24))

#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <filesystem>
#include <fstream>
#include <istream>
#include <ostream>
#include <iterator>
#include <sstream>
#include <assert.h>

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

std::vector <std::string> FindFilesOfExtension(std::string searchDir, bool use_excludes = true) {
	std::vector <std::string> res;
	for (auto& path : std::filesystem::recursive_directory_iterator(searchDir)) {
		bool bSkip = false;
		if (use_excludes) {
			for (auto& ext : exclude_extensions)
				if (ext == path.path().extension().string())
					bSkip = true;
		}

		// use bSkip if we're using the exclusion list or always save the path if not
		if (use_excludes ? bSkip : true)
		{
			res.push_back(path.path().string());
			(res.size() % 16 ? res.shrink_to_fit() : (void)0);
		}
	}
	return res;
}
std::string GetFilename(std::string fullPath, bool with_extension = true) {
	const size_t last_slash_idx = fullPath.find_last_of("\\/");
	if (std::string::npos != last_slash_idx) {
		fullPath.erase(0, last_slash_idx + 1);
	}
	if (!with_extension) {
		const size_t period_idx = fullPath.rfind('.');
		if (std::string::npos != period_idx)
			fullPath.erase(period_idx);
	}
	return fullPath;
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}

int main(int argc, char ** argp)
{
	bool bVerbose = true;

	std::string output_dir;
	if (argc == 3)
		output_dir = argp[2];

	auto files = FindFilesOfExtension(argp[1]);
	for (auto& file : files) {
		if (bVerbose)
			printf("Processing %s..\n", file.c_str());
		
		std::string output_path = file;
		replace(output_path, argp[1], output_dir);


		std::ifstream fileStream(file, std::ios::binary);
		if (fileStream.good()) {
			int totalSize = -1, num_file = 0;
			fileStream.seekg(0, std::ios::end);
			totalSize = (int)fileStream.tellg();
			fileStream.seekg(0, std::ios::beg);

			while (!fileStream.eof() && fileStream.good()) {
				Section tmpSection(fileStream, totalSize);
				if (tmpSection.data.size() > 0) {
					num_file++;

					std::string filename = GetFilename(file, false).append("_").append(std::to_string(num_file)).append(tmpSection.getExtension());
					std::ofstream output_file(std::filesystem::path(output_path).parent_path().string().append("\\" + filename).c_str(), std::ios::binary);

					std::filesystem::create_directories(std::filesystem::path(output_path).parent_path());

					if (bVerbose)
						printf("Writing to %s..\n", std::filesystem::path(output_path).parent_path().string().append("\\" + filename).c_str());

					if (output_file.good()) {
						output_file.write(reinterpret_cast<char*>(&tmpSection.data.data()[0]), tmpSection.size);
						output_file.close();
					}
				}
			}
			if (fileStream.eof() || !fileStream.good())
				continue;
		}
		fileStream.close();
	}
}