:: Place all build related files in a specific directory.
:: Whenever you'd like to clean the build and restart it from scratch, you can
:: delete this directory without worrying about deleting important files.
mkdir build-msvc15
cd build-msvc15

:: Call cmake to generate the all configured Visual Studio solution
cmake .. -G "Visual Studio 15 2017 Win64" -DQt5_DIR=E:/Qt/5.9.2/msvc2017_64/lib/cmake/Qt5

@echo off
:: Check that it run all right
if errorlevel 1 (
	echo [91mUnsuccessful[0m
) else (
	echo [92mSuccessful[0m
)
pause
