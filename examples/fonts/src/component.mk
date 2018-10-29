.PHONY: Resources.bin
	echo "THERE"
	cd ../../tools && make
	../../tools/rcomp Resources.r

COMPONENT_NAME=CreativeEngineExample
COMPONENT_PRIV_INCLUDEDIRS=../../../src
COMPONENT_EMBED_FILES := Resources.bin
