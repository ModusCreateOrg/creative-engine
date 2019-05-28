# VAR = $(wildcard ../src/*)
# $(info    VAR is ${VAR})


COMPONENT_NAME = creative-engine

CFLAGS += -DLIBXMP_CORE_PLAYER
#COMPONENT_ADD_INCLUDEDIRS= . src src/Types src/Widgets src/libxmp src/libxmp/loaders src/BStore src/BStore/OdroidStore src/Audio src/Audio/OdroidAudio src/Display src/Display/OdroidDisplay src/Controls src/Controls/OdroidControls

COMPONENT_ADD_INCLUDEDIRS= . \
	src \
	src/Types \
	src/Widgets \
	src/libxmp \
	src/libxmp/loaders \
	src/BStore \
	src/BStore/OdroidStore \
	src/Audio \
	src/Audio/OdroidAudio \
	src/Display \
	src/Display/OdroidDisplay \
	src/Controls \
	src/Controls/OdroidControls

# COMPONENT_EXTRA_INCLUDES=../src ../../src ../../../src ../../../../src
COMPONENT_SRCDIRS=${COMPONENT_ADD_INCLUDEDIRS}


