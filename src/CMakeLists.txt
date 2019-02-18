set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(
	llv_srcs
	fix.cpp
	line_length_validator.h
	validator_settings.h

)
add_library(line_length_validater_lib STATIC ${llv_srcs})
set_target_properties(line_length_validater_lib PROPERTIES LINKER_LANGUAGE CXX)

if(ENABLE_COVERAGE)
	target_link_libraries(line_length_validater_lib --coverage)
endif()

target_include_directories(line_length_validater_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})