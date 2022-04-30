/*
		(S)ection (X)tractor	- LemonHaze - 2022
*/
#include "Section.h"
#include "cpp/INIReader.h"

// Files extensions to explicitly exclude when processing files for sections
std::vector <std::string> exclude_extensions = {
	".EMU",
};

// List of Sections to extract
std::vector<SectionConfig> SectionList = {
	//               Identifier            Size Offset
	{        FOURCC('P','V','R','T'),        0            },
	{        FOURCC('D','T','P','K'),        4            },
};

void InitializeConfiguration()
{
	INIReader reader("sx.ini");
	if (!reader.ParseError())
	{
		bool	bDisablePVRT = reader.GetBoolean("general", "disable_pvrt", false),
				bDisableDTPK = reader.GetBoolean("general", "disable_dtpk", false);
		int		ini_ver = reader.GetInteger("general", "version", 0);
		printf("Config loaded from 'sx.ini': version=%d\n", ini_ver);
		if (ini_ver >= MIN_INI_VER)
			printf("Supported config version!\n");
		else
			return;

		if (bDisablePVRT)
		{
			SectionList.erase(std::remove_if(SectionList.begin(), SectionList.end(), [&](const SectionConfig& x) {
				return (x.identifier == FOURCC('P', 'V', 'R', 'T'));
			}), SectionList.end());
		}
		if (bDisableDTPK)
		{
			SectionList.erase(std::remove_if(SectionList.begin(), SectionList.end(), [&](const SectionConfig& x) {
				return (x.identifier == FOURCC('D', 'T', 'P', 'K'));
			}), SectionList.end());
		}

		// collect any section configs
		for (int i = 0; i < 0xFF; ++i) {
			std::string tmp_section_name = reader.GetString("sections", ("SectionName" + std::to_string(i)), "");
			if (tmp_section_name != "")
			{
				auto size_offset = reader.GetInteger("sections", ("SectionSize" + std::to_string(i)), -1);
				if (size_offset != -1)
				{
					SectionConfig config;
					config.identifier = ST2CC(tmp_section_name);
					config.size_offset = size_offset;
					SectionList.push_back(config);
				}
				else
					break;
			}
			else
				break;
		}

		// collect any exclude_extensions (comma-separated list... e.g. '.AAA,.BBB,.CCC')
		std::string tmp_exclude_extensions = reader.GetString("general", "exclude_extensions", "");
		if (tmp_exclude_extensions != "") {
			std::stringstream ss(tmp_exclude_extensions);
			while (ss.good()) {
				std::string extension;
				getline(ss, extension, ',');

				exclude_extensions.push_back(extension);
			}
		}
	}
}

int main(int argc, char ** argp)
{
	InitializeConfiguration();

	std::string output_dir;
	if (argc == 3)
		output_dir = argp[2];

	auto files = FindAllFilesInDirectory(argp[1]);
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