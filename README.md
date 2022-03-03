# PixelArt
Depixelizing Pixel Art

## Building
### SFML
First we need to use SFML. Either everything by default is working well on your machine and sources will be compiled once,
static libs will be built and then used by pixel art libs.
You have multiple options setting/unsetting cached variables:
	- SFML_USE_SOURCE. TRUE to build libs from source files, FALSE to use your own build
	- SFML_STATIC_LIBS. TRUE to use static libs, FALSE to use shared libs. In the latter case, do not forget to copy shared libs to executable location when trying to run program.
	- SFML_DIR. Specify the directory where custom built libs are located

The best way is to build SFML from sources in SFML/cmake. Files already present in SFML/cmake are needed, so do not delete them between consecutive builds.

add _HAS_AUTO_PTR_ETC 1