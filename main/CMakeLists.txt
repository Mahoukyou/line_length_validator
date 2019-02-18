set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(
	exec_srcs
	main.cpp
)
add_executable(exec ${exec_srcs})
set_target_properties(exec PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(exec line_length_validater_lib)

if(ENABLE_COVERAGE)
	target_link_libraries(exec --coverage)
endif()

target_include_directories(exec PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})