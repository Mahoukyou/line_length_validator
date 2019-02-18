#include "line_length_validator.h"
#include <filesystem>

// todo remove later when refactoring
#include <iostream>
#include <iterator>
#include <string>
#include <fstream>

namespace llv
{
	line_length_validator::line_length_validator(const llv::validator_settings& settings, const std::string& root_directory) :
		validator_settings_{ settings },
		root_directory_{ root_directory }
	{

	}

	void line_length_validator::validate()
	{
		// sample PoC
		// todo, refactor
		auto has_extension = [this](const std::string& str) -> bool
		{
			for (const std::string& extension : validator_settings().file_extensions_to_validate)
			{
				if (str.length() < extension.length())
				{
					continue;
				}

				if (str.find(extension, str.size() - extension.size()) != std::string::npos)
				{
					return true;
				}
			}

			return false;
		};

		std::vector<std::string> found_files_paths;
		for (auto& file_path : std::filesystem::recursive_directory_iterator(root_directory_))
		{
			if (file_path.is_regular_file() && has_extension(file_path.path().string()))
			{
				found_files_paths.push_back(file_path.path().string());
			}
		}


		std::cout << "Found " << found_files_paths.size() << " files: \n";
		std::copy(found_files_paths.begin(), found_files_paths.end(), std::ostream_iterator<std::string>(std::cout, "\n"));

		std::cout << "\n\n";
		for(const auto& file_path : found_files_paths)
		{
			std::cout << "Validating file - " << file_path << '\n';

			std::ifstream file_stream{ file_path };
			if(!file_stream.good())
			{
				std::cout << "Could not open the file, skipping it\n";
			}

			file_validator_output fvo{ file_path };

			for (std::string line; std::getline(file_stream, line);)
			{
				// todo tabulators, absolute path option	  
				if(line.length() > validator_settings().max_line_length)
				{
					fvo.add_output({ line, validator_output::e_output_type::error });
				}
				else if(line.length() > validator_settings().warning_line_length)
				{
					fvo.add_output({ line, validator_output::e_output_type::warning });
				}
			}

			if(!fvo.outputs().empty())
			{
				outputs_.push_back(std::move(fvo));
			}
		}

		std::cout << "\n\n Finished validation, found w&e in " << outputs_.size() << " files\n";
		for(const auto& el : outputs_)
		{
			std::cout << "File: " << el.file_path() << '\n';
			for(const auto& err : el.outputs())
			{
				std::cout << (err.output_type == validator_output::e_output_type::warning ? "Warning " : "Error ");
				std::cout << "line length: " << err.line.length() << " --- " << err.line << '\n';
			}

			std::cout << "\n\n";
		}
	}
}
