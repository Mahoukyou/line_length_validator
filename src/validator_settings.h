#pragma once

#include <vector>
#include <string>

struct validator_settings
{
	unsigned warning_line_length{};
	unsigned max_line_length{};

	unsigned count_tab_as_amount_of_characters{};

	bool print_file_absolute_location{};

	std::vector<std::string> file_extensions_to_validate;
};
