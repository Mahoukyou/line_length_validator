#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include "validator_settings.h"

// TODO use the error overloaded functions
// so we won't have to catch any exceptions

namespace llv
{
	enum class e_error_type 
	{ 
		warning, 
		error
	};

	enum class e_file_state_error
	{
		does_not_exist,
		not_a_file,
		cannot_open
	};

	struct file_line_error
	{
		std::wstring line;
		size_t line_number{};
		e_error_type error_type;
	};

	struct file_overview
	{
		size_t line_count{};
		size_t warning_count{};
		size_t error_count{};
	};

	class file_validator
	{
	public:
		explicit file_validator(std::filesystem::path file_path);

		~file_validator() = default;

		file_validator(const file_validator&) = default;
		file_validator(file_validator&&) noexcept = default;

		file_validator& operator=(const file_validator&) = default;
		file_validator& operator=(file_validator&&) noexcept = default;

		std::optional<e_file_state_error> validate(const validator_settings& settings);
		std::optional<e_file_state_error> update_overview(const validator_settings& settings);

		bool is_validation_cached() const noexcept
		{
			return is_validation_cached_;
		}

		bool is_overview_cached() const noexcept
		{
			return is_overview_cached_;
		}

		bool is_validation_cache_up_to_date() const;
		bool is_overview_cache_up_to_date() const;

		const std::filesystem::path& file_path() const noexcept
		{
			return file_path_;
		}

		std::wstring file_name() const;

		std::optional<e_file_state_error> file_state() const;

		const auto& results() const noexcept
		{
			return results_;
		}

		const auto& overview() const noexcept
		{
			return overview_;
		}

	private:
		static std::optional<e_error_type> validate_line(const std::wstring& line, const llv::validator_settings& settings);

		std::filesystem::path file_path_;

		std::vector<file_line_error> results_;
		file_overview overview_;

		// todo, discard the booleans and use last time modified == 0?
		bool is_validation_cached_;
		bool is_overview_cached_;

		std::filesystem::file_time_type validation_cache_last_file_modification_;
		std::filesystem::file_time_type overview_cache_last_file_modification_;
	};
}
