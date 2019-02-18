#include "line_length_validator.h"

namespace llv
{
	line_length_validator::line_length_validator(const llv::validator_settings& settings, const std::string& root_directory) :
		validator_settings_{ settings },
		root_directory_{ root_directory_ }
	{
		
	}
}