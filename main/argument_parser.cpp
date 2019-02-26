#include "argument_parser.h"

#include <cwchar>
#include <limits>
#include <iostream>

void display_help()
{
	for (auto i = 0; i < static_cast<int>(e_argument::max); ++i)
	{
		std::wcout << possible_arguments[i].data();
		switch (static_cast<e_argument>(i))
		{
		case e_argument::help:
			std::cout << " - displays all possible arguments\n";
			break;

		case e_argument::path:
			std::cout << " - path to validate, can be either path to file or a directory\n";
			break;

		case e_argument::warning_line_length:
			std::cout << " - minimum amount of characters in a line to prompt a warning\n";
			break;

		case e_argument::error_line_length:
			std::cout << " - minimum amount of characters in a line to prompt an error\n";
			break;

		case e_argument::tab_to_spaces:
			std::cout << " - amount of characters to convert tabs to during line length validation.";
			std::cout << "Settings this to n means that one tab character will be counted as n character\n";
			break;

		case e_argument::print_absolute_file_location:
			std::cout << " - display absolute file location. Set 0 disable or 1 to enable\n";
			break;

		case e_argument::file_extensions:
			std::cout << " - extensions to validate separated by a space. You can prefix them with dot or not\n";
			std::wcout << "Example usage: " << possible_arguments[i].data() << " cpp hpp .c .h\n";
			break;

		default:
			std::cout << " - ERROR, no help message was set for this argument\n";
		}
	}
}

const std::wstring_view& get_argument(const e_argument argument)
{
	return possible_arguments[static_cast<int>(argument)];
}


e_argument is_an_argument(const wchar_t* const arg)
{
	for (size_t i = 0; i < possible_arguments.size(); ++i)
	{
		if (std::wcscmp(arg, possible_arguments[i].data()) == 0)
		{
			return static_cast<e_argument>(i);
		}
	}

	return e_argument::max;
}

bool has_an_argument(const int argc, const wchar_t* const* const argv, const e_argument argument)
{
	for (auto i = 1; i < argc; ++i)
	{
		if (std::wcscmp(argv[i], get_argument(argument).data()) == 0)
		{
			return true;
		}
	}

	return false;
}

std::optional<unsigned> parse_to_uint(const wchar_t* const arg)
{
	// It is kinda crappy, because we don't get an explicit answer if the string was converted without an issue
	// We get either 0 or ULONG_MAX, so if we pass an L"0", the return value is ambiguous (same for ULONG_MAX)

	// For this console app, lets just assume that if we get ULONG_MAX it was an error (converted value fell out of range of unsigned long)
	// Because any of our argument values should not even exceed 16bit integer
	// And that if we get 0, we will check if the first character is equal to L'0'
	// Damn, wide chars causing so many problems

	// todo, write own function for conversion?

	wchar_t *dummy{};
	const auto result = wcstoul(arg, &dummy, 10);

	if(result == std::numeric_limits<decltype(result)>::max())
	{
		return std::nullopt;
	}

	if(result == 0 && wcsncmp(arg, L"0", 1) != 0)
	{
		return std::nullopt;
	}

	// We even have to convert to unsigned, because there are only functions for long/ulong
	if(result > std::numeric_limits<unsigned>::max())
	{
		return std::nullopt;
	}

	return { result };
}

parsed_arguments parse_arguments(const int argc, const wchar_t* const* const argv)
{
	// todo refactor errors
	parsed_arguments pa{ L"", {80, 100, 4, false, {}} };
	for (auto i = 1; i < argc; ++i)
	{
		if (const auto argument = is_an_argument(argv[i]);
			argument != e_argument::max)
		{
			switch (argument)
			{
			case e_argument::help:
				display_help();
				break;

			case e_argument::path:
				// TODO, should I leave the check if the next string is a different argument
				// or just simply take it as the path?
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == e_argument::max)
				{
					pa.file_path = argv[i + 1];
					// todo take path until we encounter another argument (because path may contain a space?)

					++i;
					break;
				}

				std::cout << "err with --path arg\n"; // todo
				break;

			case e_argument::warning_line_length:
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == e_argument::max)
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

			case e_argument::error_line_length:
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == e_argument::max)
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

			case e_argument::tab_to_spaces:
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == e_argument::max)
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

			case e_argument::print_absolute_file_location:
				if (i + 1 < argc && is_an_argument(argv[i + 1]) == e_argument::max)
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

			case e_argument::file_extensions:
				while(i + 1 < argc && is_an_argument(argv[i + 1]) == e_argument::max)
				{
					// todo,  duplicates
					if ((argv[i + 1])[0] == '.')
					{
						pa.settings.file_extensions_to_validate.emplace_back(argv[i + 1]);
					}
					else
					{
						std::wstring extension{ L'.' };
						extension += argv[i + 1];

						pa.settings.file_extensions_to_validate.push_back(std::move(extension));
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
			std::wcout << "argument - [" << argv[i] << "] not recognized\n";
		}
	}

	return pa;
}
