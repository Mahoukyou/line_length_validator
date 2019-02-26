#pragma once

#include "validator_settings.h"
#include "file_validator.h"

#include <filesystem>
#include <string>

#ifndef _WIN32
	static_assert(0, "Only Windows is supported at the moment");
#endif

namespace llv
{
	class line_length_validator
	{
	public:
		line_length_validator(validator_settings settings, std::filesystem::path path);
		~line_length_validator() = default;

		line_length_validator(const line_length_validator&) = delete;
		line_length_validator(line_length_validator&&) noexcept = delete;

		line_length_validator& operator=(const line_length_validator&) = delete;
		line_length_validator& operator=(line_length_validator&&) noexcept = delete;

		const std::filesystem::path& path() const noexcept
		{
			return path_;
		}

		const validator_settings& validator_settings() const noexcept
		{
			return validator_settings_;
		}

		const auto& file_validators() const noexcept
		{
			return file_validators_;
		}

		auto& file_validator(const size_t index)
		{
			return file_validators_[index];
		}

		const auto& file_validator(const size_t index) const
		{
			return file_validators_[index];
		}

		bool is_path_valid() const
		{
			return exists(path()) && (is_regular_file(path()) || is_directory(path()));
		}

		void validate(size_t index);
		void validate(bool update_directory_files = true);

		void update_overview(size_t index);
		void update_overview();

		void update_files_in_directory();

	private:
		std::vector<std::filesystem::path> files_to_validate() const;

		std::vector<llv::file_validator> file_validators_;

		llv::validator_settings validator_settings_;
		std::filesystem::path path_;
	};
}