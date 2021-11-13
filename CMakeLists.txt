cmake_minimum_required(VERSION 3.5)
set(PROJECT jedi)
project(${PROJECT} LANGUAGES C)

include_directories(include)
set(SOURCES
    src/main.c
    src/menu.c
    src/test.c
    src/timer2.c
    src/tools.c)
set(HEADERS
    include/menu.h
    include/test.h
    include/timer2.h
    include/tools.h)

add_definitions(-Wall)
add_executable(${PROJECT} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT} rt)
