:: Place all build related files in a specific directory.
:: Whenever you'd like to clean the build and restart it from scratch, you can
:: delete this directory without worrying about deleting important files.
mkdir build-mingw
cd build-mingw

:: Call cmake to generate the MinGW solution, check the path to your Qt
:: If Qt is not found, you can specify its location using e.g. -DQt5_DIR=E:/Qt/5.9.2/mingw53_32/lib/cmake/Qt5
cmake .. -G "MinGW Makefiles"

@echo off
:: Check that it run all right
if errorlevel 1 (
	echo [91mUnsuccessful[0m
) else (
	echo [92mSuccessful[0m
	echo "You can now run 'mingw32-make' in directory 'build-mingw'"
)
pause
