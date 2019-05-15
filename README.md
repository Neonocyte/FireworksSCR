# FireworksSCR
A particle demo utilizing SDL2, and built to be used as a screensaver. Draws a flare that bursts into a number of sparks from a .xpm image file. Compatible with multiple monitors.

## Setup
This project requires an installation of SDL2 and SDL2_image to be compiled.
The included MakefileTemplate is built for Windows users utilizing MinGW64. In this setup, place the DLLs for SDL2 and SDL2_image in the root directory of the project. In the MakefileTemplate, modify the INCLUDE and LIB variables to the location of the respective folders in the SDL2 download. Use "make windows" to make a binary for Windows systems.

Other systems will need to follow the instructions provided for their system on the SDL wiki at http://wiki.libsdl.org/Installation to install the required libraries. Use "make all" to make a binary for non-Windows systems.

A debug make command is provided using the "make debug" command.

To use as a Windows screensaver, rename the output .exe file to end with the .scr file ending. Afterwards, right click the file and select "Install". At the current time, no configuration or preview is supported.

## Technical Details
FireworksSCR uses the .xpm image file format to load images. When used as a screensaver, the root directory may be changed, so it is necessary for any images to be packaged with the binary. A .xpm image packages the image as C code, and can be generated using an image editor such as Krita. To use a .xpm, include the image like you would a header file; the image is stored in an array that is defined in the .xpm file. For the included image, "fireworks" is the array containing the image. This data can then be used by SDL2_image.

The image used by FireworksSCR uses an 8x8 spritesheet.

Screensavers are launched using the "-S" argument for the main program, "-C" for configuration, or "-P" for preview.

Multi-monitor support is achieved by creating a single borderless window sized to stretch across all monitors.
