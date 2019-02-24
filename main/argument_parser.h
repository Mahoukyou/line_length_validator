#pragma once

#include <array>
#include <string_view>
#include <optional>

#include "validator_settings.h"

enum e_argument
{
	argument_help,
	argument_path,
	argument_warning_line_length,
	argument_error_line_length,
	argument_tab_to_spaces,
	argument_print_absolute_file_location,
	argument_file_extensions,
	argument_max
};

/* it's just easier to use the same wide chars because we will have to do comparisons with passed arguments anyway */
inline const std::array<std::wstring_view, argument_max> possible_arguments{
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

e_argument is_an_argument(const wchar_t* arg);
bool has_an_argument(int argc, const wchar_t *const *argv, e_argument argument);

std::optional<unsigned> parse_to_uint(const wchar_t* arg);
parsed_arguments parse_arguments(int argc, const wchar_t *const *argv);