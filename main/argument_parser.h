#pragma once

#include <array>
#include <string_view>
#include <optional>

#include "validator_settings.h"

enum class e_argument
{
	help,
	path,
	warning_line_length,
	error_line_length,
	tab_to_spaces,
	print_absolute_file_location,
	file_extensions,
	max
};

/* it's just easier to use the same wide chars because we will have to do comparisons with passed arguments anyway */
inline const std::array<std::wstring_view, static_cast<int>(e_argument::max)> possible_arguments{
	L"--help",
	L"-path",
	L"-wlen",
	L"-elen",
	L"-tsize",
	L"-absolute",
	L"-extensions",
};

struct parsed_arguments // todo, rename to something more appropriate
{
	std::wstring file_path;
	llv::validator_settings settings;
};

void display_help();

const std::wstring_view& get_argument(e_argument argument);
e_argument is_an_argument(const wchar_t* arg);
bool has_an_argument(int argc, const wchar_t *const *argv, e_argument argument);

std::optional<unsigned> parse_to_uint(const wchar_t* arg);
std::optional<parsed_arguments> parse_arguments(int argc, const wchar_t *const *argv);