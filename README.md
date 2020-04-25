# TinyTIFF

This is an RGB version of the original TinyTIFF code found at https://github.com/jkriege2/TinyTIFF.  Please see the readme file there for an indepth descritpion of the original code.

The speed that this code affords is importatant to the time lapse toll I'm writting.  For this project it was necessary to allow 16-bit RGB data which was not difficult to change.  In additions, I also needed read the EXIF data.  Please see main.cpp for an exampe about how to the access EXIF data.  The EXIF codes needed to read that data are in the TinyTIFFReader.hpp.  Because of these addidions, a C++ compilee is required.
