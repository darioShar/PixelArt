# PixelArt
Depixelizing Pixel Art

## Building
### SFML
First we need to use SFML. Either everything by is working well by default on your machine : sources will be compiled once, static libs will be built and then linked.
You have multiple other options accessible by setting/unsetting cached variables:

- SFML_USE_SOURCE. TRUE to build libs from source files, FALSE to use your own build
- SFML_STATIC_LIBS. TRUE to use static libs, FALSE to use shared libs. In the latter case, do not forget to copy shared libs to executable location when trying to run program.
- EXTLIBS_INCLUDE and EXTLIBS_LIBS. If using static libs from your own build, don't forget to specify extlibs location as they are used and need by sfml.
- SFML_DIR. Specify the directory where custom built libs are located


The best way is to build SFML from sources in SFML/cmake. Files already present in SFML/cmake are needed, so do not delete them between consecutive builds.

If auto_ptr bug in SFML source code, add ```#define _HAS_AUTO_PTR_ETC 1``` to beginning of specified source file.
