#pragma once

#include <vector>
#include <string>

namespace llv
{
	struct validator_settings
	{
		unsigned warning_line_length{};
		unsigned error_line_length{};

		unsigned count_tab_as_amount_of_characters{};

		bool print_file_absolute_location{};

		std::vector<std::wstring> file_extensions_to_validate;
	};
}