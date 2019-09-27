:: Get git submodules
:: This only has to run once, you can then comment it out if it takes too long
git submodule update --init --recursive

:: Place all build related files in a specific directory.
:: Whenever you'd like to clean the build and restart it from scratch, you can
:: delete this directory without worrying about deleting important files.
mkdir build-mingw
cd build-mingw

:: Call cmake to generate the MinGW solution
:: If Qt is not found, you can specify its location using e.g. -DQt5_DIR=E:/Qt/5.9.2/msvc2017_64/lib/cmake/Qt5
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DQt5_DIR=E:/Qt/5.12.1/5.12.1/msvc2017_64/lib/cmake/Qt5

@echo off
:: Check that it run all right
if errorlevel 1 (
	echo [91mUnsuccessful[0m
) else (
	echo [92mSuccessful[0m
	echo "You can now run 'mingw32-make' in directory 'build-mingw'"
)
pause
