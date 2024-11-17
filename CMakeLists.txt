cmake_minimum_required(VERSION 3.0)
project(httpC C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(HOMEBREW_DIR "/opt/homebrew/include/")


include_directories(${HOMEBREW_DIR} ${SRC_DIR})
link_directories(${MPEGS_LIBRARY_DIRS})

add_executable(http_server
  src/main.c
)

if(APPLE)
    target_link_libraries(http_server 
        ${CMAKE_DL_LIBS} 
    )
else()
    target_link_libraries(http_server
      ${CMAKE_DL_LIBS}
    )
endif()
