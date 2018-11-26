# VAR = $(wildcard ../src/*)
# $(info    VAR is ${VAR})

CFLAGS += -DLIBXMP_CORE_PLAYER

COMPONENT_PRIV_INCLUDEDIRS=. src src/Types src/Widgets src/libxmp src/libxmp/loaders
# COMPONENT_EXTRA_INCLUDES=../src ../../src ../../../src ../../../../src
COMPONENT_SRCDIRS=${COMPONENT_PRIV_INCLUDEDIRS}

