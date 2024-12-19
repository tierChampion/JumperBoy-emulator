# JumperBoy-emulator
GameBoy Emulator using SDL2 for rendering and audio, and Imgui for the user interface. 
Works on both linux and windows.

## Compilation

### Linux

For a basic compilation, simply compile this like a regular cmake project:

$ cmake -B build  
$ make -C build  

You can also use the cmake tools in VScode to build other configurations.

### Windows

To properly compile this application on windows, you must first give it access to the SDL2.dll in the libs/SDL2 folder.
To do this, add a new path to the PATH environnement variable that points to the .dll file in this project. Optionnaly, you 
can simply move the .dll file in the folder containing the .exe after compiling.  

For the audio to work, you might need to set the SDL_AUDIODRIVER environnement variable to a sensible value like directsound.  

To compile this application, simply compile it like a cmake project in microsoft visual studio 2022. For now, only the x64 versions 
are working.

## Tests

Passes all the basic Blargg test for instructions, the acid2 tests by Math Currie.

Behaviour that is known to not be correct / tested:
- OBJ priority in CGB mode
- RAM for MBC1
- MBC3 & MBC5

### Games

Some games that have issues are:
- Pokemon Crystal (text doesn't render correctly)
- All MBC5 games (not implemented yet)
