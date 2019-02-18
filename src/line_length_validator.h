#pragma once

#include "validator_settings.h"
#include "validator_output.h"
#include <string>

namespace llv
{
	class line_length_validator
	{
	public:
		line_length_validator(const validator_settings& settings, const std::string& root_directory);
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

		// todo for now
		void validate();

	private:
		std::vector<llv::file_validator_output> outputs_;
		llv::validator_settings validator_settings_;
		std::string root_directory_;
	};
}