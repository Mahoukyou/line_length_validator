#pragma once

#include <string>
#include <vector>
#include <array>

namespace llv
{
	enum class e_error_type 
	{ 
		warning, 
		error,
		none
	};

	struct validator_output
	{
		std::string line;
		e_error_type error_type;
	};

	struct validator_file_overview
	{
		std::string file_name;
		std::array<size_t, 2> error_count{};

		size_t line_count{};
		bool is_valid{ true }; // todo rename more appropriately
	};

	class file_validator_output
	{
	public:
		explicit file_validator_output(const std::string& file_path);

		~file_validator_output() = default;

		file_validator_output(const file_validator_output&) = default;
		file_validator_output(file_validator_output&&) noexcept = default;

		file_validator_output& operator=(const file_validator_output&) = default;
		file_validator_output& operator=(file_validator_output&&) noexcept = default;

		void add_output(const validator_output& output)
		{
			outputs_.push_back(output);
		}

		void add_output(validator_output&& output)
		{
			outputs_.push_back(std::move(output));
		}

		const std::string& file_path() const noexcept
		{
			return file_path_;
		}

		const auto& outputs() const noexcept
		{
			return outputs_;
		}

	private:
		std::string file_path_;
		std::vector<validator_output> outputs_;
	};
}