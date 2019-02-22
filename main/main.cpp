#include "line_length_validator.h"
#include <iostream>	
#include <windows.h>
#include <array>
#include <optional>
#include <charconv>

// todo, unix 
class console_color
{
public:
	enum : unsigned char
	{
		color_red = 12,
		color_green = 10,
		color_yellow = 14,
	};

	explicit constexpr console_color(const WORD color) :
		color_{color}
	{
		
	}

	friend std::ostream& operator<<(std::ostream& os, const console_color& cc)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cc.color_);
		return os;
	}


private:
	const WORD color_;
};

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

constexpr std::array<std::string_view, argument_max> possible_arguments{ 
	"--help", 
	"-path",
	"-wlen",
	"-elen",
	"-tsize",
	"-absolute",
	"-extensions",
};

console_color get_default_console_color()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi_info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi_info);

	return console_color{ csbi_info.wAttributes };
}

struct parsed_arguments
{
	std::string file_path;
	llv::validator_settings settings;
};

void print_validator(const llv::file_validator& file_validator)
{
	
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

e_argument is_an_argument(const char* const arg)
{
	for (size_t i = 0; i < possible_arguments.size(); ++i)
	{
		if(strcmp(arg, possible_arguments[i].data()) == 0)
		{
			return static_cast<e_argument>(i);
		}
	}

	return argument_max;
}

parsed_arguments parse_arguments(const int argc, const char * const * const argv)
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
				if(i + 1 < argc && is_an_argument(argv[i+1]) == argument_max)
				{
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

bool has_help_argument(const int argc, const char *const *const argv)
{
	for(int i = 1; i < argc; ++i)
	{
		if(std::strcmp(argv[i], possible_arguments[argument_help].data()) == 0)
		{
			return true;
		}
	}

	return false;
}

int main(int argc, char** argv)
{
	using namespace llv;

	if (has_help_argument(argc, argv))
	{
		display_help();
		return 0;
	}

	// filename, arg_type, arg_value
	if (argc < 3)
	{
		std::cout << "You have to pass at least a path to a file or directory.\n";
		std::cout << "Use " << possible_arguments[argument_path].data() << " [path] to do so.\n";
		std::cout << "Use --help to see all available launch arguments\n";
		return 1;
	}

	parse_arguments(argc, argv);

	// console colors
	constexpr console_color cred{ console_color::color_red }, cgreen{ console_color::color_green }, cyellow{ console_color::color_yellow };
	const console_color cdefault{ get_default_console_color() };


	const auto arguments = parse_arguments(argc, argv);
	line_length_validator llv{ arguments.settings, arguments.file_path };

	llv.validate();

	for(const auto& file_validator : llv.file_validators())
	{
		if(file_validator.results().empty())
		{
		//	continue;
		}

		std::cout << "File: " << file_validator.file_name() << cgreen <<  "\nLines: " << file_validator.overview().line_count << cdefault;
		std::cout << cyellow << "\t Warnings: " << file_validator.overview().warning_count << cdefault;
		std::cout << cred << "\t Errors: " << file_validator.overview().error_count << cdefault << "\n";

		for(const auto& error : file_validator.results())
		{
			std::cout << cdefault << "Line: " << error.line_number << "\t Line length: " << error.line.size() << "\tError Type: ";
			switch(error.error_type)
			{
			case e_error_type::warning:
				std::cout << cyellow << "warning";
				break;

			case e_error_type::error:
				std::cout << cred << "error";
				break;

			default:
				break;
			}

			std::cout << '\n' << error.line << cdefault << '\n';
		}

		std::cout << '\n';
	}
}
