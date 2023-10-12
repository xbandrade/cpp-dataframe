@echo off

if exist build rmdir /s /q build
mkdir build
cd build
cmake ..
cmake --build .
cd ..
copy build\Debug\cpp-df.exe CppDf.exe
CppDf.exe
