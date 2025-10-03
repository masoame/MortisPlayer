@echo off
mkdir out
cd out
cmake -S ../ -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build  build --config Debug
pause