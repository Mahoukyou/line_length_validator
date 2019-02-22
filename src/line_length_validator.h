#pragma once

#include "validator_settings.h"
#include "file_validator.h"
#include <string>

namespace llv
{
	class line_length_validator
	{
	public:
		line_length_validator(validator_settings settings, std::string root_directory);
		~line_length_validator() = default;

		line_length_validator(const line_length_validator&) = delete;
		line_length_validator(line_length_validator&&) noexcept = delete;

		line_length_validator& operator=(const line_length_validator&) = delete;
		line_length_validator& operator=(line_length_validator&&) noexcept = delete;

		const std::string& root_directory() const noexcept
		{
			return root_directory_;
		}

		const validator_settings& validator_settings() const noexcept
		{
			return validator_settings_;
		}

		const auto& file_validators() const noexcept
		{
			return file_validators_;
		}

		void validate(bool update_directory_files = true);
		void update_files_overview();
		void update_files_in_directory();

	private:
		std::vector<std::string> files_to_validate() const;

		std::vector<llv::file_validator> file_validators_;

		llv::validator_settings validator_settings_;
		std::string root_directory_;
	};
}