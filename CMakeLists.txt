cmake_minimum_required(VERSION 3.10)
project(SmartServe VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

file(GLOB SOURCES 
    "src/*.cpp"
)

add_executable(SmartServe ${SOURCES})

# Windows specific console subsystem
if(WIN32)
    target_compile_definitions(SmartServe PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()
