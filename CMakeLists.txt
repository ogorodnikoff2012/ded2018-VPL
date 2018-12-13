cmake_minimum_required(VERSION 3.8)
project(VPL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -O0 -ggdb3 -Wall -Wextra -fsanitize=address,undefined -D_GLIBCXX_DEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra")

include_directories(include)

# set(LIBRARY_SRC )

# add_library(vpl STATIC ${LIBRARY_SRC})
add_executable(vpl_front src/vpl_front.cpp)
add_executable(vpl_back  src/vpl_back.cpp)

# target_link_libraries(vpl_front vpl)
# target_link_libraries(vpl_back vpl)
