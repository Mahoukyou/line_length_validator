#include "argument_parser.h"

e_argument is_an_argument(const char* const arg)
{
	for (size_t i = 0; i < possible_arguments.size(); ++i)
	{
		if (strcmp(arg, possible_arguments[i].data()) == 0)
		{
			return static_cast<e_argument>(i);
		}
	}

	return argument_max;
}

std::optional<int> parse_to_int(const char* const arg)
{
	const auto arg_end = strchr(arg, '\0');

	int parsed_value{ 0 };
	const auto parse_result = std::from_chars(arg, arg_end, parsed_value);
	if (parse_result.ec != std::errc())
	{
		return std::nullopt;
	}

	return { parsed_value };
}

void display_help()
{
	for (int i = 0; i < argument_max; ++i)
	{
		std::cout << possible_arguments[i].data();
		switch (i)
		{
		case argument_help:
			std::cout << " - displays all possible arguments\n";
			break;

		case argument_path:
			std::cout << " - path to validate, can be either path to file or a directory\n";
			break;

		case argument_warning_line_length:
			std::cout << " - minimum amount of characters in a line to prompt a warning\n";
			break;

		case argument_error_line_length:
			std::cout << " - minimum amount of characters in a line to prompt an error\n";
			break;

		case argument_tab_to_spaces:
			std::cout << " - amount of characters to convert tabs to during line length validation.";
			std::cout << "Settings this to n means that one tab character will be counted as n character\n";
			break;

		case argument_print_absolute_file_location:
			std::cout << " - display absolute file location. Set 0 disable or 1 to enable\n";
			break;

		case argument_file_extensions:
			std::cout << " - extensions to validate separated by a space. You can prefix them with dot or not\n";
			std::cout << "Example usage: " << possible_arguments[i].data() << " cpp hpp .c .h\n";
			break;

		default:
			std::cout << " - ERROR, no help message was set for this argument\n";
		}
	}
}

bool has_help_argument(const int argc, const char* const* const argv)
{
	for (int i = 1; i < argc; ++i)
	{
		if (std::strcmp(argv[i], possible_arguments[argument_help].data()) == 0)
		{
			return true;
		}
	}

	return false;
}

parsed_arguments parse_arguments(const int argc, const char* const* const argv)
{
	parsed_arguments pa{ "", {80, 100, 4, false, {}} };
	for (int i = 1; i < argc; ++i)
	{
		if (const auto argument = is_an_argument(argv[i]);
			argument != argument_max)
		{
			switch (argument)
			{
			case argument_help:
				display_help();
				break;

			case argument_path:
				// TODO, should I leave the check if the next string is a different argument
				// or just simply take it as the path?
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == argument_max)
				{
					const auto ar = argv[i + 1];
					pa.file_path = argv[i + 1];
					// todo take path until we encounter another argument (because path may contain a space?)
					break;
				}

				std::cout << "err with --path arg\n"; // todo
				break;

			case argument_warning_line_length:
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == argument_max)
				{
					if (const auto warning_length = parse_to_int(argv[i + 1]);
						warning_length.has_value())
					{
						pa.settings.warning_line_length = warning_length.value();
					}
					else
					{
						std::cout << "coulnd parse war\n";
					}
				}

				std::cout << "warl err\n"; // todo
				break;

			case argument_error_line_length:
				break;

			case argument_tab_to_spaces:
				break;

			case argument_print_absolute_file_location:
				break;

			case argument_file_extensions:
				break;

			default:
				std::cout << " - ERROR, parse was not set for this argument\n";
			}
		}
		else
		{

		}
	}

	return pa;
}
