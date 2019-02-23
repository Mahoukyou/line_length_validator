#pragma once

#include <string>
#include <vector>
#include <optional>
#include <filesystem>
#include "validator_settings.h"

namespace llv
{
	enum class e_error_type 
	{ 
		warning, 
		error
	};

	struct file_line_error
	{
		std::string line; // todo, consider wstring
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

		bool validate(const validator_settings& settings);
		bool update_overview(const validator_settings& settings);

		bool is_validated() const noexcept
		{
			return is_validated_;
		}

		bool is_overview_updated() const noexcept
		{
			return is_overview_updated_;
		}

		const std::filesystem::path& file_path() const noexcept
		{
			return file_path_;
		}

		std::wstring file_name() const;

		const auto& results() const noexcept
		{
			return results_;
		}

		const auto& overview() const noexcept
		{
			return overview_;
		}

	private:
		static std::optional<e_error_type> validate_line(const std::string& line, const llv::validator_settings& settings);

		std::filesystem::path file_path_;

		std::vector<file_line_error> results_;
		file_overview overview_;

		bool is_validated_;
		bool is_overview_updated_;
	};
}
