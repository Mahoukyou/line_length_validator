#include "line_length_validator.h"

int main()
{
	using namespace llv;

	const std::string test_path{ R"(C:\Users\Dawid Wdowiak\Desktop\Development\VisualizedSorting)" };

	const validator_settings settings{ 100, 120, 4, false, {".cpp", ".h"} };
	line_length_validator llv{ settings, test_path};

}