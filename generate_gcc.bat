@echo off
md out
md out\gcc
cd out\gcc
cmake -G Ninja ..\..
pause
