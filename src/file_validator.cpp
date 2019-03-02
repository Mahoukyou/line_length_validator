#include "file_validator.h"
#include <fstream>
#include <algorithm>

namespace llv
{
	file_validator::file_validator(std::filesystem::path file_path) :
		file_path_{ std::move(file_path) },
		is_validation_cached_{ false },
		is_overview_cached_{ false }
	{

	}

	std::optional<e_file_state_error> file_validator::validate(const validator_settings& settings)
	{
		if (const auto file_state_error = file_state();
			file_state_error.has_value())
		{
			return file_state_error;
		}

		const auto last_file_modification = last_write_time(file_path());

		std::wifstream file_stream{ file_path() };
		if (!file_stream.good())
		{
			return e_file_state_error::cannot_open;
		}

		file_overview new_overview{};
		std::vector<file_line_error> new_results;
		new_results.reserve(results().size());

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

		is_validation_cached_ = true;
		is_overview_cached_ = true;
		validation_cache_last_file_modification_ = last_file_modification;
		overview_cache_last_file_modification_ = last_file_modification;

		return std::nullopt;
	}

	std::optional<e_file_state_error> file_validator::update_overview(const validator_settings& settings)
	{
		if (const auto file_state_error = file_state();
			file_state_error.has_value())
		{
			return file_state_error;
		}

		const auto last_file_modification = last_write_time(file_path());

		overview_ = file_overview{};
		std::wifstream file_stream{ file_path() };
		if (!file_stream.good())
		{
			return e_file_state_error::cannot_open;
		}

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

		is_overview_cached_ = true;
		overview_cache_last_file_modification_ = last_file_modification;

		return std::nullopt;
	}

	bool file_validator::is_validation_cache_up_to_date() const
	{
		if (!is_validation_cached() || file_state().has_value())
		{
			return false;
		}

		return validation_cache_last_file_modification_ == last_write_time(file_path());
	}

	bool file_validator::is_overview_cache_up_to_date() const
	{
		if (!is_overview_cached() || file_state().has_value())
		{
			return false;
		}

		return overview_cache_last_file_modification_ == last_write_time(file_path());
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

	std::optional<e_file_state_error> file_validator::file_state() const
	{
		if(!exists(file_path()))
		{
			return e_file_state_error::does_not_exist;
		}

		if(!is_regular_file(file_path()))
		{
			return e_file_state_error::not_a_file;
		}

		return std::nullopt;
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