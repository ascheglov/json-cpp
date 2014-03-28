@echo off
md out
md out\vc
cd out\vc
cmake -G "Visual Studio 12" ..\..
pause
