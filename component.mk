# VAR = $(wildcard ../src/*)
# $(info    VAR is ${VAR})

CFLAGS += -DLIBXMP_CORE_PLAYER -DLIBXMP_CORE_DISABLE_IT

COMPONENT_PRIV_INCLUDEDIRS=src src/libxmp src/libxmp/loaders
# COMPONENT_EXTRA_INCLUDES=../src ../../src ../../../src ../../../../src
COMPONENT_SRCDIRS=${COMPONENT_PRIV_INCLUDEDIRS}

