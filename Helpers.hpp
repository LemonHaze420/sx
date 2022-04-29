#pragma once
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

#define FOURST(x)		 std::string(reinterpret_cast<char*>(&x), 4)
#define FOURCC(a,b,c,d)	 (signed int)((char)(a & 0xFF) << 24 | (char)(b & 0xFF) << 16 | (char)(c & 0xFF) << 8 | (char)(d & 0xFF))
#define FLIP(x)			 (signed int)(( x >> 24 ) | (( x << 8) & 0x00ff0000 )| ((x >> 8) & 0x0000ff00) | ( x << 24))

struct SectionConfig;

extern std::vector <std::string> exclude_extensions;
extern std::vector<SectionConfig> SectionList;

static std::vector <std::string> FindFilesOfExtension(std::string searchDir, bool use_excludes = true) {
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
static std::string GetFilename(std::string fullPath, bool with_extension = true) {
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

static bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}
