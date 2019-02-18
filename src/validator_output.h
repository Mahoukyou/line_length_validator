#pragma once

#include <string>
#include <vector>

namespace llv
{
	struct validator_output
	{
		enum class e_output_type { warning, error };

		std::string line;
		e_output_type output_type;
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