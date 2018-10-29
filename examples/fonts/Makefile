# ESP-IDF Makefile for game project

PROJECT_NAME=GENUS_FONT_EXAMPLE
EXTRA_COMPONENT_DIRS=${PROJECT_PATH}/creative-engine ${PROJECT_PATH}/src

include $(IDF_PATH)/make/project.mk

rcomp: FORCE
	echo "Building rcomp"
	cd ../../tools && make

resources: rcomp FORCE
	echo "Compiling resources"
	cd src && ../../../tools/rcomp Resources.r

FORCE:

