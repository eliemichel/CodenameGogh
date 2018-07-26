Gogh
====

Build
-----

### Windows 64bit with Visual Studio 15 (2017)

Run `build-msvc15.bat`, then open `build-msvc15/RayStep.sln`.

### Other

	mkdir build
	cd build
	cmake .. # or e.g. cmake .. -G "MinGW" to use mingw
	make # or open generated solution (Visual Studio, XCode, etc.)
