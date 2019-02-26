#include "line_length_validator.h"
#include "console_color.h"
#include "argument_parser.h"
#include <iostream>	

#ifndef _WIN32
	static_assert(0, "Only Windows is supported at the moment");
#endif

void print_validation_info(const llv::file_validator& file_validator)
{
	std::wcout << "File: " << file_validator.file_name();
	std::cout << cgreen << "\nLines: " << file_validator.overview().line_count << cdefault;
	std::cout << cyellow << "\t Warnings: " << file_validator.overview().warning_count << cdefault;
	std::cout << cred << "\t Errors: " << file_validator.overview().error_count << cdefault << "\n";

	for (const auto& error : file_validator.results())
	{
		std::cout << cdefault << "Line: " << error.line_number << "\t Line length: " << error.line.size() << "\tError Type: ";
		switch (error.error_type)
		{
		case llv::e_error_type::warning:
			std::cout << cyellow << "warning";
			break;

		case llv::e_error_type::error:
			std::cout << cred << "error";
			break;

		default:
			break;
		}

		std::wcout << '\n' << error.line << cdefault << '\n';
	}

	std::cout << '\n';
}

int wmain(const int argc, wchar_t** argv)
{
	if (has_an_argument(argc, argv, e_argument::help))
	{
		display_help();
		return 0;
	}
	
	if (constexpr auto min_arguments_required{ 3 }; 
		argc < min_arguments_required)
	{
		std::cout << "You have to pass at least a path to a file or directory.\n";
		std::wcout << "Use " << get_argument(e_argument::path).data() << " [path] to do so.\n";
		std::cout << "Use --help to see all available launch arguments\n";
		return 1;
	}

	auto parse_result = parse_arguments(argc, argv);
	if(!parse_result.has_value())
	{
		return 2;
	}

	llv::line_length_validator llv{ std::move(parse_result.value().settings), std::move(parse_result.value().file_path) };

	llv.validate();
	for (const auto& file_validator : llv.file_validators())
	{
		if (file_validator.results().empty())
		{
			continue;
		}

		print_validation_info(file_validator);
	}
}