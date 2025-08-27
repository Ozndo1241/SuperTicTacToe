cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

project(tictactoe_ui LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)
file(GLOB SRC_FILES src/*.cpp)


find_package(wxWidgets REQUIRED COMPONENTS core base adv propgrid)
include(${wxWidgets_USE_FILE})

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

set(wxBUILD_SHARED OFF)
