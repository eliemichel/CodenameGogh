Gogh
====
## Presentation
Gogh is a cross-platform nodal based interface for **[ffmpeg](https://ffmpeg.org/)** transcoder.
It is born from the love of **ffmpeg**'s power and the hate of writing every time same commands. Its name is as it is because it mixes streams as innovative as some famous painter did many years ago.

### Main purpose
Gogh is meant to be **ergonomic** and **operational**. It must be understandable in few seconds and the user mustn't struggle with anything. And, most of all, we want to avoid never-ending lists you scroll again and again to find the right preset to encode with...  
We chose to base it on **ffmpeg** because it's a full and powerful tool and we can focus on the design.

### Storytelling
Élie and Félix are having dinner in an italian restaurant, eating something absolutely not pizzas.  
Suddenly, they look at each other and start speaking almost at the same time.
- **Félix :** It's been a long time I want to make a nodal tool for transcoding with ffmpeg.
- **Élie :** It's been a long time I want to make a nodal framework.

Silence... A car passes and a dog barks.

- **Together :** Let's do it !

Gogh was born.

Build
-----

### Windows 64bit with Visual Studio 15 (2017)

Run `build-msvc15.bat`, then open `build-msvc15/RayStep.sln`.

### Other

	mkdir build
	cd build
	cmake .. # or e.g. cmake .. -G "MinGW" to use mingw
	make # or open generated solution (Visual Studio, XCode, etc.)
