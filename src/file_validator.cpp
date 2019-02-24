#include "file_validator.h"
#include <fstream>
#include <algorithm>

namespace llv
{
	file_validator::file_validator(std::filesystem::path file_path) :
		file_path_{ std::move(file_path) },
		is_validated_{ false },
		is_overview_updated_{ false }
	{

	}

	bool file_validator::validate(const validator_settings& settings)
	{
		std::wifstream file_stream{ file_path_ };
		if (!file_stream.good())
		{
			// set it to false, but let's leave the old results intact.
			is_validated_ = false;
			return false;
		}

		file_overview new_overview{};
		std::vector<file_line_error> new_results;
		new_results.reserve(results().size());

		// todo, wstring?
		size_t line_index{ 1 };
		for (std::wstring line; std::getline(file_stream, line); ++line_index)
		{
			if (const auto error_type = validate_line(line, settings);
				error_type.has_value())
			{
				// todo, emplace back
				new_results.push_back({ std::move(line), line_index, error_type.value() });

				switch (error_type.value())
				{
				case e_error_type::warning:
					++new_overview.warning_count;
					break;

				case e_error_type::error:
					++new_overview.error_count;
					break;

				default:
					break;
				}
			}
		}

		new_overview.line_count = line_index - 1;

		results_ = std::move(new_results);
		overview_ = new_overview;

		is_validated_ = true;
		is_overview_updated_ = true;

		return true;
	}

	bool file_validator::update_overview(const validator_settings& settings)
	{
		overview_ = file_overview{};

		std::wifstream file_stream{ file_path() };
		if (!file_stream.good())
		{
			is_overview_updated_ = false;
			return false;
		}

		// todo, wstring?
		for (std::wstring line; std::getline(file_stream, line);)
		{
			++overview_.line_count;
			if (const auto error_type = validate_line(line, settings);
				error_type.has_value())
			{
				switch (error_type.value())
				{
				case e_error_type::warning:
					++overview_.warning_count;
					break;

				case e_error_type::error:
					++overview_.error_count;
					break;

				default:
					break;
				}
			}
		}

		is_overview_updated_ = true;

		return true;
	}

	std::wstring file_validator::file_name() const
	{
		auto file_name = file_path().wstring();

		const auto last_slash_index1 = file_name.find_last_of('\\');
		const auto last_slash_index2 = file_name.find_last_of('/');

		size_t file_name_begin_index{};
		if (last_slash_index1 == std::string::npos)
		{
			file_name_begin_index = last_slash_index2;
		}
		else if (last_slash_index2 == std::string::npos)
		{
			file_name_begin_index = last_slash_index1;
		}
		else
		{
			file_name_begin_index = std::max(last_slash_index1, last_slash_index2);
		}

		return file_name.substr(file_name_begin_index + 1);
	}

	std::optional<e_error_type> file_validator::validate_line(const std::wstring& line, const llv::validator_settings& settings)
	{
		auto line_length = line.size();
		if (settings.count_tab_as_amount_of_characters != 1)
		{
			const auto tab_count = std::count(line.begin(), line.end(), L'\t');
			line_length += tab_count * settings.count_tab_as_amount_of_characters - tab_count;
		}

		if (line_length > settings.error_line_length)
		{
			return e_error_type::error;
		}

		if (line_length > settings.warning_line_length)
		{
			return e_error_type::warning;
		}

		return std::nullopt;
	}
}