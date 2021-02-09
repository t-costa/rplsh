cmake_minimum_required (VERSION 2.8.11)

project (rpl-shell)

option(EXPANSION "Expansion of trees enabled" OFF)

if(EXPANSION)
	add_definitions(-DEXPANSION)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-std=c++17 -g -O0 -pedantic -Wall -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-std=c++17 -O3 -Wall")

add_subdirectory (rpl)
add_executable (rplsh rplsh.cpp)
target_link_libraries (rplsh LINK_PUBLIC rpl)
target_link_libraries(rplsh LINK_PUBLIC readline)


# possible useful warnings flag
# -pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self
# -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast
# -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo
# -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Werror -Wno-unused
