#pragma once

#include <ostream>
#include <windows.h>

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
		color_{ color }
	{

	}

	friend std::ostream& operator<<(std::ostream& os, const console_color& cc)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cc.color_);
		return os;
	}

	friend std::wostream& operator<<(std::wostream& os, const console_color& cc)
	{
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), cc.color_);
		return os;
	}

private:
	const WORD color_;
};

inline console_color get_default_console_color()
{
	CONSOLE_SCREEN_BUFFER_INFO console_screen_buffer_info;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &console_screen_buffer_info);

	return console_color{ console_screen_buffer_info.wAttributes };
}

inline console_color cred{ console_color::color_red };
inline console_color cgreen{ console_color::color_green };
inline console_color cyellow{ console_color::color_yellow };
inline console_color cdefault{ get_default_console_color() };