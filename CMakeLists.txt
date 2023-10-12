cmake_minimum_required(VERSION 3.10)

project(cpp-df)

add_executable(cpp-df main.cpp)

set(HEADER_FILES
    CSVRow.hpp
    utils.hpp
    dataframe.hpp
)

set(SOURCE_FILES
    CSVRow.cpp
    utils.cpp
    dataframe.cpp
)


target_sources(cpp-df PRIVATE
    ${SOURCE_FILES}
    ${HEADER_FILES}
)

target_compile_definitions(cpp-df PRIVATE __STDC_WANT_SECURE_LIB__)

target_include_directories(cpp-df PRIVATE
    "D:/Dev/vcpkg/packages/nlohmann-json_x86-windows/include"
)
