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

	void line_length_validator::update_files_overview()
	{
		// todo cache?
		// redo later, for now quick poc, update all every time
		const auto files_paths = files_to_validate();

		// todo, just work on the original one?, leave it for now in case we will want strong exception guarantee
		std::vector<validator_file_overview> new_files_overview;
		new_files_overview.reserve(files_paths.size());

		for (const auto& file_path : files_paths)
		{
			validator_file_overview vfo;
			vfo.file_name = file_path; // todo get name later, not important atm

			std::ifstream file_stream{ file_path };
			if (!file_stream.good())
			{
				vfo.is_valid = false;
			}
			else
			{
				for (std::string line; std::getline(file_stream, line);)
				{
					++vfo.line_count;
					if (const auto error = validate_line(line); error != e_error_type::none)
					{
						++vfo.error_count[static_cast<int>(error)];
					}
				}
			}

			new_files_overview.push_back(std::move(vfo));
		}

		files_overview_ = std::move(new_files_overview);
	}

	void line_length_validator::validate()
	{
		const auto found_files_paths = files_to_validate();

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
				if(const auto error = validate_line(line); error != e_error_type::none)
				{
					fvo.add_output({ line, error });
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
				std::cout << (err.error_type == e_error_type::warning ? "Warning " : "Error ");
				std::cout << "line length: " << err.line.length() << " --- " << err.line << '\n';
			}

			std::cout << "\n\n";
		}
	}

	std::vector<std::string> line_length_validator::files_to_validate() const
	{
		const auto has_extension = [this](const std::string& str) -> bool
		{
			for (const auto& extension : validator_settings().file_extensions_to_validate)
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

		// todo, maybe use directory_entry instead of to get files/store them?
		// refactor later in case we need to check if our data is dirty instead of revalidating everything
		std::vector<std::string> found_files_paths;
		for (auto& directory_entry : std::filesystem::recursive_directory_iterator(root_directory()))
		{
			if (directory_entry.is_regular_file() && has_extension(directory_entry.path().string()))
			{
				found_files_paths.push_back(directory_entry.path().string());
			}
		}

		return found_files_paths;
	}

	e_error_type line_length_validator::validate_line(const std::string& line) const
	{
		size_t line_length = line.size();
		if (validator_settings().count_tab_as_amount_of_characters == 1)
		{
			const auto tab_count = std::count(line.begin(), line.end(), '\t');
			line_length += tab_count * validator_settings().count_tab_as_amount_of_characters - tab_count;
		}

		if (line_length > validator_settings().max_line_length)
		{
			return e_error_type::error;
		}

		if (line_length > validator_settings().warning_line_length)
		{
			return e_error_type::warning;
		}

		return e_error_type::none;
	}
}
