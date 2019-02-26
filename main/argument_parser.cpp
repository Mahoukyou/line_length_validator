#include "argument_parser.h"

#include <cwchar>
#include <limits>
#include <iostream>
#include <filesystem>

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
	const auto is_next_argument_an_value = [&](const int current_index) -> bool
	{
		const auto next_index = current_index + 1;
		return next_index < argc && is_an_argument(argv[next_index]) == e_argument::max;
	};

	// todo, rename more appropriately, sound like we are going to look for next uint value, and not just check the next value
	const auto set_to_next_uint_argument = [&](unsigned& destination, int& argument_index) -> bool
	{
		if (is_next_argument_an_value(argument_index))
		{
			if (const auto parsed_uint = parse_to_uint(argv[argument_index + 1]);
				parsed_uint.has_value())
			{
				destination = parsed_uint.value();
			}
			else
			{
				std::wcout << "[" << argv[argument_index] << "] - Failed to parse argument value [";
				std::wcout << argv[argument_index + 1] << "] to unsigned int\n";

				++argument_index;
				return false;
			}

			++argument_index;
		}
		else
		{
			std::wcout << "[" << argv[argument_index] << "] - is not associated with any value\n";
			return false;
		}

		return true;
	};

	bool parse_succeeded{ true };

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
				if (is_next_argument_an_value(i))
				{
					pa.file_path = argv[i + 1];
					// todo take path until we encounter another argument (because path may contain a space?)

					++i;
					break;
				}

				parse_succeeded = false;

				std::cout << "err with --path arg\n"; // todo
				break;

			case e_argument::warning_line_length:
				if (!set_to_next_uint_argument(pa.settings.warning_line_length, i))
				{
					parse_succeeded = false;
				}

				break;

			case e_argument::error_line_length:
				if (!set_to_next_uint_argument(pa.settings.error_line_length, i))
				{
					parse_succeeded = false;
				}

				break;

			case e_argument::tab_to_spaces:
				if (!set_to_next_uint_argument(pa.settings.count_tab_as_amount_of_characters, i))
				{
					parse_succeeded = false;
				};

				break;

			case e_argument::print_absolute_file_location:
			{
				unsigned dummy{};
				if (!set_to_next_uint_argument(dummy, i))
				{
					parse_succeeded = false;
					break;
				};

				if (dummy == 0 || dummy == 1)
				{
					pa.settings.print_file_absolute_location = dummy == 1;
				}
				else
				{
					std::wcout << "[" << argv[i] << "] - invalid value. Can accept only 0 or 1";
				}

				break;
			}

			case e_argument::file_extensions:
			{
				auto& extensions_vector = pa.settings.file_extensions_to_validate;
				while (is_next_argument_an_value(i))
				{
					if (wcsncmp(argv[i + 1], L".", 1) == 0)
					{
						extensions_vector.emplace_back(argv[i + 1]);
					}
					else
					{
						std::wstring extension{ L'.' };
						extension += argv[i + 1];

						extensions_vector.push_back(std::move(extension));
					}

					++i;
				}

				// We could have just used a set, but vector is better for data locality
				// And we will be iterating over it quite a lot
				if (extensions_vector.size() > 1)
				{
					std::sort(extensions_vector.begin(), extensions_vector.end());
					const auto one_past_unique_it = std::unique(extensions_vector.begin(), extensions_vector.end());
					extensions_vector.erase(one_past_unique_it, extensions_vector.end());
					extensions_vector.shrink_to_fit();
				}

				break;
			}
			default:
				std::wcout << "[" << argv[i] << "] - unexpected error, parse was not set for this argument";
				parse_succeeded = false;
			}
		}
		else
		{
			std::wcout << "[" << argv[i] << "] - is not a recognized argument\n";
			parse_succeeded = false;
		}
	}

	// Do the check in here when we have path (hopefully)
	// Extensions are not needed if we are checking a single file
	if (pa.settings.file_extensions_to_validate.empty() &&
		std::filesystem::exists(pa.file_path) &&
		!std::filesystem::is_regular_file(pa.file_path))
	{
		std::cout << "Could not find any extensions to parse\n";
		parse_succeeded = false;
	}

	// todo, return value;

	return pa;
}