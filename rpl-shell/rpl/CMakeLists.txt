# Create library
project (rpl)

include_directories(.)

# Add subdirectories
add_subdirectory (nodes)
add_subdirectory (parser)
add_subdirectory (visitors)
add_subdirectory (rewriting)
add_subdirectory (environment)
add_subdirectory (error_report)
add_subdirectory (cpp_parser)

# Add source file
add_library(rpl interpreter.cpp history.cpp tab_completion.cpp)

# Target link libraries
target_link_libraries(rpl parser visitors rewriting cpp_parser)

# Make sure the compiler can find include files
target_include_directories (rpl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
