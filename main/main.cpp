#include "line_length_validator.h"
#include <iostream>	
#include <windows.h>

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

	constexpr console_color(const WORD color) :
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

console_color get_default_console_color()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi_info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi_info);

	return { csbi_info.wAttributes };
}

int main()
{
	using namespace llv;

	constexpr console_color cred(console_color::color_red), cgreen(console_color::color_green), cyellow(console_color::color_yellow);
	const console_color cdefault{ get_default_console_color() };

	const std::string test_path{ R"(K:\godot)" };

	const validator_settings settings{ 200, 250, 4, false, {".cpp", ".h"} };
	line_length_validator llv{ settings, test_path };

	llv.validate();

	for(const auto& file_validator : llv.file_validators())
	{
		if(file_validator.results().empty())
		{
			continue;
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
