cmake_minimum_required (VERSION 3.2)

project (line_length_validator)

if(NOT WIN32)
	#wide char paths, wfstream being only supported on msvc, differently sized wide chars, wmain(...)
	#gave me a headache, so for the time being this lib & console app is only for windows
	#might refactor for unix later
	message(FATAL_ERROR "LLV is only supported on Windows OS for the time being.")
endif()

#Coverage
if(CMAKE_COMPILER_IS_GNUCC)
	option(ENABLE_COVERAGE "Enable coverage" FALSE)

	if(ENABLE_COVERAGE)
		add_compile_options(--coverage -O0)
	endif()
endif()

include_directories(".")
add_subdirectory(src)
add_subdirectory(main)