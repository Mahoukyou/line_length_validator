#include "argument_parser.h"

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

bool has_an_argument(const int argc, const char* const* const argv, const e_argument argument)
{
	for (int i = 1; i < argc; ++i)
	{
		if (std::strcmp(argv[i], possible_arguments[argument].data()) == 0)
		{
			return true;
		}
	}

	return false;
}

std::optional<unsigned> parse_to_uint(const char* const arg)
{
	const auto arg_end = strchr(arg, '\0');

	unsigned parsed_value{ 0 };
	const auto parse_result = std::from_chars(arg, arg_end, parsed_value);
	if (parse_result.ec != std::errc())
	{
		return std::nullopt;
	}

	return { parsed_value };
}

parsed_arguments parse_arguments(const int argc, const char* const* const argv)
{
	// todo refactor errors
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
					pa.file_path = argv[i + 1];
					// todo take path until we encounter another argument (because path may contain a space?)

					++i;
					break;
				}

				std::cout << "err with --path arg\n"; // todo
				break;

			case argument_warning_line_length:
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == argument_max)
				{
					if (const auto warning_length = parse_to_uint(argv[i + 1]);
						warning_length.has_value())
					{
						pa.settings.warning_line_length = warning_length.value();
					}
					else
					{
						std::cout << "coulnd parse war\n";
					}

					++i;
					break;
				}

				std::cout << "warl err\n"; // todo
				break;

			case argument_error_line_length:
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == argument_max)
				{
					if (const auto error_length = parse_to_uint(argv[i + 1]);
						error_length.has_value())
					{
						pa.settings.error_line_length = error_length.value();
					}
					else
					{
						std::cout << "coulnd parse err\n";
					}

					++i;
					break;
				}

				std::cout << "errl err\n"; // todo
				break;

			case argument_tab_to_spaces:
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == argument_max)
				{
					if (const auto chars_per_tabulator = parse_to_uint(argv[i + 1]);
						chars_per_tabulator.has_value())
					{
						pa.settings.count_tab_as_amount_of_characters = chars_per_tabulator.value();
					}
					else
					{
						std::cout << "coulnd parse tabs\n";
					}

					++i;
					break;
				}

				std::cout << "tabs err\n"; // todo
				break;

			case argument_print_absolute_file_location:
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == argument_max)
				{
					if (const auto result = parse_to_uint(argv[i + 1]);
						result.has_value())
					{
						if (result == 0 || result == 1)
						{
							pa.settings.print_file_absolute_location = static_cast<bool>(result);
						}
						else
						{
							std::cout << "invalid value";
						}
					}
					else
					{
						std::cout << "coulnd parse tabs\n";
					}

					++i;
					break;
				}

				std::cout << "tabs err\n"; // todo
				break;

			case argument_file_extensions:
				while(i + 1 < argc && is_an_argument(argv[i + 1]) == argument_max)
				{
					// todo,  duplicates
					if ((argv[i + 1])[0] == '.')
					{
						pa.settings.file_extensions_to_validate.emplace_back(argv[i + 1]);
					}
					else
					{
						std::string extenstion{ '.' };
						extenstion += argv[i + 1];

						pa.settings.file_extensions_to_validate.push_back(std::move(extenstion));
					}

					++i;
				}

				if(pa.settings.file_extensions_to_validate.empty())
				{
					std::cout << "coulnd't find/parse extenstions\n";
				}

				break;

			default:
				std::cout << " - ERROR, parse was not set for this argument\n";
			}
		}
		else
		{
			std::cout << "argument - [" << argv[i] << "] not recognized\n";
		}
	}

	return pa;
}
