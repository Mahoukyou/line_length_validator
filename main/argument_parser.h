#pragma once

#include <array>
#include <string_view>
#include <charconv>
#include <optional>

#include "validator_settings.h"
#include <iostream>

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

inline const std::array<std::string_view, argument_max> possible_arguments{
	"--help",
	"-path",
	"-wlen",
	"-elen",
	"-tsize",
	"-absolute",
	"-extensions",
};

struct parsed_arguments // todo, rename to something more appropriate
{
	std::string file_path;
	llv::validator_settings settings;
};

void display_help();

e_argument is_an_argument(const char* arg);
bool has_an_argument(int argc, const char *const *argv, e_argument argument);

std::optional<unsigned> parse_to_uint(const char* arg);
parsed_arguments parse_arguments(int argc, const char *const *argv);