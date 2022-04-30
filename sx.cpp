/*
		(S)ection (X)tractor	- LemonHaze - 2022
*/
#include "Section.h"

// Files extensions to explicitly exclude when processing files for sections
std::vector <std::string> exclude_extensions = {
	".EMU",
};

// List of Sections to extract
std::vector<SectionConfig> SectionList = {
	//               Identifier            Size Offset
	//{        FOURCC('P','V','R','T'),        0            },
	{        FOURCC('D','T','P','K'),        4            },
};

int main(int argc, char ** argp)
{
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
			auto fileSize = GetFileSize(fileStream);

			while (!fileStream.eof() && fileStream.good()) {
				Section tmpSection(fileStream, fileSize);

				if (tmpSection.getSize() > 0) 
				{
					num_file++;

					std::string filename = GetFilename(file, false).append("_").append(std::to_string(num_file)).append(tmpSection.getExtension());
					
					std::string path = std::filesystem::path(output_path).parent_path().string().append("\\" + filename).c_str();
					std::filesystem::create_directories(std::filesystem::path(output_path).parent_path());
					
					std::ofstream output_file(path, std::ios::binary);
					if (output_file.good()) {
						printf("Writing to %s..\n", path.c_str());
						output_file.write(reinterpret_cast<const char*>(tmpSection.getData().data()), tmpSection.getData().size());
						output_file.close();
						++num_completed;
					} 
					else 
					{
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

	printf("All tasks completed with %d errors. (%d out of %d with %lld total files).\n", num_failed, num_completed, num_file, files.size());

	return 0;
}